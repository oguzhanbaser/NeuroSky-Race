////////////////////////////////////////////////////////////////////////
// Arduino Bluetooth Interface with Brainsense
//
// This is example code provided by Pantech Prolabs. and is provided
// license free.
////////////////////////////////////////////////////////////////////////
#include <Arduino.h>
#include <SoftwareSerial.h>
#include <EEPROM.h>
#include "configuration.h"

// checksum variables
byte generatedChecksum = 0;
byte checksum = 0;
int payloadLength = 0;
byte payloadData[64] = { 0 };
byte poorQuality = 200, lastPoorQuality = 200;
byte attention = 0, lastAttention = 0;
byte battery = 0;
byte meditation = 0;
int motorVal = 0;

// system variables
long lastReceivedPacket = 0;
boolean bigPacket = false;
bool ready = false, ready_in = false, stop_race = false;
unsigned long lastTime1 = 0, lastTime2 = 0;
int ledArray[LED_ARRAY_SIZE] = { GREENLED1, GREENLED2, GREENLED3, YELLOWLED1, YELLOWLED2,
YELLOWLED3, YELLOWLED4, REDLED1, REDLED2, REDLED3 };
uint8_t turSayisi = 0;

//software Serial
#ifdef USE_EXTERNAL_USB
SoftwareSerial usbSer(SOFT_SERIAL_RX, SOFT_SERIAL_TX);
#endif

//speed levels
uint8_t speedArray[ARRAY_SIZE];

#ifdef USE_MZ80
void tur_int()
{
	turSayisi++;

	//if lap count equals to ten stop the race
	if (turSayisi > 10)
	{
		stop_race = true;
		ready = false;
	}
}
#endif

//////////////////////////
// Microprocessor Setup //
//////////////////////////
void setup()
{
#ifdef USE_EXTERNAL_USB
	usbSer.begin(DEBUG_BAUDRATE);
#endif
	Serial.begin(BAUDRATE);           // USB

	//need for proteus simulation runnig 
#ifdef MAKE_SIM
	setSpeedArray();
#endif

	//need for first uploading
#ifdef WRITE_EEPROM
	setSpeedArray();

	for (int i = 0; i < ARRAY_SIZE; i++)
	{
		EEPROM.write(i, speedArray[i]);
	}
#endif

	//need for reading eeprom values
#ifdef DEBUG_EEPROM
	for (int i = 0; i < ARRAY_SIZE; i++)
	{
		speedArray[i] = EEPROM.read(i);
		Serial.print("Val: ");
		Serial.print(i);
		Serial.print(" ");
		Serial.println(speedArray[i]);
	}
#endif

	//speed val initialize
	for (int i = 0; i < ARRAY_SIZE; i++)
	{
		speedArray[i] = EEPROM.read(i);
	}

	//attention leve indicators
	pinMode(GREENLED1, OUTPUT);
	pinMode(GREENLED2, OUTPUT);
	pinMode(GREENLED3, OUTPUT);
	pinMode(YELLOWLED1, OUTPUT);
	pinMode(YELLOWLED2, OUTPUT);
	pinMode(YELLOWLED3, OUTPUT);
	pinMode(YELLOWLED4, OUTPUT);
	pinMode(REDLED1, OUTPUT);
	pinMode(REDLED2, OUTPUT);
	pinMode(REDLED3, OUTPUT);
	pinMode(MOTOR_OUT, OUTPUT);

	//Players ready?
	pinMode(READY_IN, INPUT);
	pinMode(READY_OUT, OUTPUT);

#ifdef USE_MZ80
	attachInterrupt(digitalPinToInterrupt(MZ_80), tur_int, FALLING);
#endif

	//poor quality indicator led
	pinMode(LED, OUTPUT);
}

////////////////////////////////
// Read data from Serial UART //
////////////////////////////////
byte ReadOneByte()

{
	int ByteRead;
	while (!Serial.available());
	lastTime2 = millis();
	ByteRead = Serial.read();

#if DEBUGOUTPUT
	Serial.print((char)ByteRead);   // echo the same byte out the USB serial (for debug purposes)
#endif

	return ByteRead;
}

/////////////
//MAIN LOOP//
/////////////
void loop()
{

#ifndef MAKE_GAME
	ready = true;
	ready_in = true;
#else
	ready_in = digitalRead(READY_IN);
#endif

	digitalWrite(READY_OUT, ready);


	if (millis() - lastTime2 > TIMEOUT)
	{
		lastPoorQuality = 200;
		lastAttention = 0;
		lastTime2 = millis();
#ifdef USE_EXTERNAL_USB
		Serial.println("Timeout reached");
#endif
		for (int i = 0; i < LED_ARRAY_SIZE; i++)
		{
			digitalWrite(ledArray[i], LOW);
		}
	}

	//If the race end stop the cars
#ifdef MAKE_GAME
	if (stop_race)
	{
		analogWrite(MOTOR_OUT, 0);
	}
#endif

	/*
	if (usbSer.available())
	{
		char c = usbSer.read();

		int index = 'a' - c;

		int val = usbSer.parseInt();

		if ((index <= 10) && (index >= 0 ) && 
			(val <= 255) && (val >= 0))
		{
			speedArray[index] = val;
			EEPROM.write(index, speedArray[index]);
		}
	}*/

	if (Serial.available())

	{
		// Look for sync bytes
		if (ReadOneByte() == 170)
		{
			if (ReadOneByte() == 170)
			{
				payloadLength = ReadOneByte();

				if (payloadLength > 169)                     //Payload length can not be greater than 169
					return;
				generatedChecksum = 0;
				for (int i = 0; i < payloadLength; i++)
				{
					payloadData[i] = ReadOneByte();            //Read payload into memory
					generatedChecksum += payloadData[i];
				}

				checksum = ReadOneByte();                      //Read checksum byte from stream
				generatedChecksum = 255 - generatedChecksum;   //Take one's compliment of generated checksum


				if (checksum == generatedChecksum)
				{
					poorQuality = 200;
					attention = 0;
					meditation = 0;
					lastTime2 = millis();

					for (int i = 0; i < payloadLength; i++)
					{ // Parse the payload
						switch (payloadData[i])
						{
						case 2:
							i++;
							poorQuality = payloadData[i];
							lastPoorQuality = poorQuality;
							bigPacket = true;
							break;
						case 3:
							i++;
							battery = payloadData[i];
							break;
						case 4:
							i++;
							attention = payloadData[i];
							lastAttention = attention;
							break;
						case 5:
							i++;
							meditation = payloadData[i];
							break;
						case 0x80:
							i = i + 3;
							break;
						case 0x83:
							i = i + 25;
							break;
						default:
							break;
						} // switch
					} // for loop

#if !DEBUGOUTPUT

					if (bigPacket)
					{
						if (poorQuality == 0)
						{
							digitalWrite(LED, HIGH);
							ready = true;				//veri d�zge�n gelmi�se cihaz� haz�ra getir
						}
						else
						{
							attention = 0;
							digitalWrite(LED, LOW);
							ready = false;				//veri d�zg�n gelmemi�se haz�rdan ��kar
						}	

#ifdef USE_EXTERNAL_USB
						Serial.print("Signal Quality: ");
						Serial.print(poorQuality, DEC);
						Serial.print(" Meditation: ");
						Serial.print(meditation, DEC);
						Serial.print(" Attention: ");
						Serial.print(attention, DEC);
						Serial.print(" Time since last packet: ");
						Serial.print(millis() - lastReceivedPacket, DEC);
						lastReceivedPacket = millis();
						Serial.println();
#endif

						switch (attention / 10)
						{
						case 0:
							digitalWrite(GREENLED1, HIGH);
							digitalWrite(GREENLED2, LOW);
							digitalWrite(GREENLED3, LOW);
							digitalWrite(YELLOWLED1, LOW);
							digitalWrite(YELLOWLED2, LOW);
							digitalWrite(YELLOWLED3, LOW);
							digitalWrite(YELLOWLED4, LOW);
							digitalWrite(REDLED1, LOW);
							digitalWrite(REDLED2, LOW);
							digitalWrite(REDLED3, LOW);
							motorVal = speedArray[0];
							break;
						case 1:
							digitalWrite(GREENLED1, HIGH);
							digitalWrite(GREENLED2, HIGH);
							digitalWrite(GREENLED3, LOW);
							digitalWrite(YELLOWLED1, LOW);
							digitalWrite(YELLOWLED2, LOW);
							digitalWrite(YELLOWLED3, LOW);
							digitalWrite(YELLOWLED4, LOW);
							digitalWrite(REDLED1, LOW);
							digitalWrite(REDLED2, LOW);
							digitalWrite(REDLED3, LOW);
							motorVal = speedArray[1];
							break;
						case 2:
							digitalWrite(GREENLED1, HIGH);
							digitalWrite(GREENLED2, HIGH);
							digitalWrite(GREENLED3, HIGH);
							digitalWrite(YELLOWLED1, LOW);
							digitalWrite(YELLOWLED2, LOW);
							digitalWrite(YELLOWLED3, LOW);
							digitalWrite(YELLOWLED4, LOW);
							digitalWrite(REDLED1, LOW);
							digitalWrite(REDLED2, LOW);
							digitalWrite(REDLED3, LOW);
							motorVal = speedArray[2];
							break;
						case 3:
							digitalWrite(GREENLED1, HIGH);
							digitalWrite(GREENLED2, HIGH);
							digitalWrite(GREENLED3, HIGH);
							digitalWrite(YELLOWLED1, HIGH);
							digitalWrite(YELLOWLED2, LOW);
							digitalWrite(YELLOWLED3, LOW);
							digitalWrite(YELLOWLED4, LOW);
							digitalWrite(REDLED1, LOW);
							digitalWrite(REDLED2, LOW);
							digitalWrite(REDLED3, LOW);
							motorVal = speedArray[3];
							break;
						case 4:
							digitalWrite(GREENLED1, HIGH);
							digitalWrite(GREENLED2, HIGH);
							digitalWrite(GREENLED3, HIGH);
							digitalWrite(YELLOWLED1, HIGH);
							digitalWrite(YELLOWLED2, HIGH);
							digitalWrite(YELLOWLED3, LOW);
							digitalWrite(YELLOWLED4, LOW);
							digitalWrite(REDLED1, LOW);
							digitalWrite(REDLED2, LOW);
							digitalWrite(REDLED3, LOW);
							motorVal = speedArray[4];
							break;
						case 5:
							digitalWrite(GREENLED1, HIGH);
							digitalWrite(GREENLED2, HIGH);
							digitalWrite(GREENLED3, HIGH);
							digitalWrite(YELLOWLED1, HIGH);
							digitalWrite(YELLOWLED2, HIGH);
							digitalWrite(YELLOWLED3, HIGH);
							digitalWrite(YELLOWLED4, LOW);
							digitalWrite(REDLED1, LOW);
							digitalWrite(REDLED2, LOW);
							digitalWrite(REDLED3, LOW);
							motorVal = speedArray[5];
							break;
						case 6:
							digitalWrite(GREENLED1, HIGH);
							digitalWrite(GREENLED2, HIGH);
							digitalWrite(GREENLED3, HIGH);
							digitalWrite(YELLOWLED1, HIGH);
							digitalWrite(YELLOWLED2, HIGH);
							digitalWrite(YELLOWLED3, HIGH);
							digitalWrite(YELLOWLED4, HIGH);
							digitalWrite(REDLED1, LOW);
							digitalWrite(REDLED2, LOW);
							digitalWrite(REDLED3, LOW);
							motorVal = speedArray[6];
							break;
						case 7:
							digitalWrite(GREENLED1, HIGH);
							digitalWrite(GREENLED2, HIGH);
							digitalWrite(GREENLED3, HIGH);
							digitalWrite(YELLOWLED1, HIGH);
							digitalWrite(YELLOWLED2, HIGH);
							digitalWrite(YELLOWLED3, HIGH);
							digitalWrite(YELLOWLED4, HIGH);
							digitalWrite(REDLED1, HIGH);
							digitalWrite(REDLED2, LOW);
							digitalWrite(REDLED3, LOW);
							motorVal = speedArray[7];
							break;
						case 8:
							digitalWrite(GREENLED1, HIGH);
							digitalWrite(GREENLED2, HIGH);
							digitalWrite(GREENLED3, HIGH);
							digitalWrite(YELLOWLED1, HIGH);
							digitalWrite(YELLOWLED2, HIGH);
							digitalWrite(YELLOWLED3, HIGH);
							digitalWrite(YELLOWLED4, HIGH);
							digitalWrite(REDLED1, HIGH);
							digitalWrite(REDLED2, HIGH);
							digitalWrite(REDLED3, LOW);
							motorVal = speedArray[8];
							break;
						case 9:
							digitalWrite(GREENLED1, HIGH);
							digitalWrite(GREENLED2, HIGH);
							digitalWrite(GREENLED3, HIGH);
							digitalWrite(YELLOWLED1, HIGH);
							digitalWrite(YELLOWLED2, HIGH);
							digitalWrite(YELLOWLED3, HIGH);
							digitalWrite(YELLOWLED4, HIGH);
							digitalWrite(REDLED1, HIGH);
							digitalWrite(REDLED2, HIGH);
							digitalWrite(REDLED3, HIGH);
							motorVal = speedArray[9];
							break;
						case 10:
							digitalWrite(GREENLED1, HIGH);
							digitalWrite(GREENLED2, HIGH);
							digitalWrite(GREENLED3, HIGH);
							digitalWrite(YELLOWLED1, HIGH);
							digitalWrite(YELLOWLED2, HIGH);
							digitalWrite(YELLOWLED3, HIGH);
							digitalWrite(YELLOWLED4, HIGH);
							digitalWrite(REDLED1, HIGH);
							digitalWrite(REDLED2, HIGH);
							digitalWrite(REDLED3, HIGH);
							motorVal = speedArray[10];
							break;
						}

						if (ready && ready_in)
						{
							motorVal = motorVal;
						}
						else{
							//analogWrite(MOTOR_OUT, 0);
							for (int i = 0; i < ARRAY_SIZE; i++)
							{
								digitalWrite(ledArray[i], LOW);
							}
							motorVal = 0;
						}
					}
#endif
					bigPacket = false;
				}
				else {
					// Checksum Error
				}  // end if else for checksum
			} // end if read 0xAA byte
		} // end if read 0xAA byte
	}

	//send siqnal quality and attention level per second
	if (millis() - lastTime1 > INTERVAL)
	{
		String ss = "#|" + String(lastPoorQuality) + "|" + String(lastAttention);
#ifdef USE_EXTERNAL_USB
		usbSer.println(ss);
#endif

#ifndef USE_EXTERNAL_USB
		Serial.println(ss);
#endif
		lastTime1 = millis();
	}

	analogWrite(MOTOR_OUT, motorVal);
}

//default speed array
void setSpeedArray()
{
	speedArray[0] = 0;
	speedArray[1] = 0;
	speedArray[2] = 0;
	speedArray[3] = 0;
	speedArray[4] = 90;
	speedArray[5] = 95;
	speedArray[6] = 100;
	speedArray[7] = 105;
	speedArray[8] = 110;
	speedArray[9] = 120;
	speedArray[10] = 130;
}
