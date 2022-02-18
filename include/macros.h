#ifndef __MACROS_H
#define __MACROS_H


//debug
#define DEBUG 0
#define COMMENT 1
#define CODE_NOT_IN_USE 0


// Default Definitions and LED's
#define LED_BLINKER GPIO_NUM_2
#define BLINK_RATE 1000
#define maxDaysToExpire 30

#define noNetwork 600
#define connectedToServer 1000
long del_DB;
long interval = noNetwork; 
int ongoingTask = 0;
int ActiveAtTrans = 0;

//Input Command definitions
#define READSMS "aaaa" //1,2,3,4,5
#define DAILUSSD "bbbb"
#define GSMSTATUS "cccc"
#define RESTARTGSM "dddd"
#define FEEDBACK "eeee"
#define CHECKBAL "ffff"
#define TESTCOMM "zzzz"
#define CMD_LEN 4

//Socketio Server Event Endpoints
#define READSMS_ADDR "handle_sms"
#define gettask "gettask"
#define DAILUSSD_ADDR "handle_ussd_status"
#define GSMSTATUS_ADDR ""
#define RESTARTGSM_ADDR ""

//serial communication
#define HARD_BAUD 9600
// #define RXD0 3
// #define TXD0 1
#define RXD1 18
#define TXD1 19
#define RXD2 16
#define TXD2 17


#define TERMINATION_CHAR '\n'


//variables

int ledState = LOW;
unsigned long previousMillis = 0;


#define DebugPrint Serial
#define sim1 1
#define sim2 2
#define sim3 3


#include "BluetoothSerial.h"
#include <errno.h>
#include "cmd.h"


  ATcommander SimSlot1;
  ATcommander SimSlot2;
  ATcommander SimSlot3;

#include "simcomm.h"

#endif