//#define WRITE_EEPROM 
#define DEBUG_EEPROM
//#define USE_EXTERNAL_USB
//#define MAKE_SIM
#define MAKE_GAME               //uncomment it if you are make test

#ifdef MAKE_GAME
#define USE_MZ80
#endif

#define LED 13                  //siqnal quality indicator led
#define BAUDRATE 57600          //do not change this. This baud rate NeuroSky' s baud rate
#define DEBUGOUTPUT 0
#ifdef USE_EXTERNAL_USB
#define DEBUG_BAUDRATE 9600
#endif

//attention level indicator led pins
#define GREENLED1  4
#define GREENLED2  5
#define GREENLED3  6
#define YELLOWLED1 7
#define YELLOWLED2 8
#define YELLOWLED3 9
#define YELLOWLED4 10
#define REDLED1    11
#define REDLED2    12
#define REDLED3    A3

/*
    Ready In and Ready Out requires to check are the players connected?
    If this player' s poor quality equals to 0, Ready out will set high
    If the other player' s poor quality equals to 0, its arduino will set own ready out pin to high
    So we can check is the other player connected
*/
#define READY_IN   A1
#define READY_OUT  A0

//Software Serial pins
#define SOFT_SERIAL_RX A4
#define SOFT_SERIAL_TX A5

#define MOTOR_OUT 3         //mosfet gate pin

#define ARRAY_SIZE 11

#define MZ_80	2

#define INTERVAL 500
#define TIMEOUT	3000

#define LED_ARRAY_SIZE 10