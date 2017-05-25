//#define WRITE_EEPROM 
#define DEBUG_EEPROM
//#define USE_EXTERNAL_USB
//#define MAKE_SIM
#define MAKE_GAME

#ifdef MAKE_GAME
#define USE_MZ80
#endif

#define LED 13
#define BAUDRATE 57600
#define DEBUGOUTPUT 0
#ifdef USE_EXTERNAL_USB
#define DEBUG_BAUDRATE 9600
#endif

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


#define READY_IN   A1
#define READY_OUT  A0

#define SOFT_SERIAL_RX A4
#define SOFT_SERIAL_TX A5

#define MOTOR_OUT 3

#define ARRAY_SIZE 11

#define MZ_80	2

#define INTERVAL 500
#define TIMEOUT	3000

#define LED_ARRAY_SIZE 10