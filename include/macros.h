#ifndef __MACROS_H
#define __MACROS_H


//debug
#define DEBUG             0
#define COMMENT           1
#define CODE_NOT_IN_USE   0


// Default Definitions and LED's
#define LED_BLINKER GPIO_NUM_2
unsigned long pingTime = 0;
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


#define USE_SERIAL Serial
#define sim1 1
#define sim2 2
#define sim3 3


#define UNSOL_MSG "+CMTI"


#include "BluetoothSerial.h"
#include <errno.h>
// #include "cmd.h"


#define SimSlot1  Serial // NEAR THE POWER MODULE
#define SimSlot2  Serial2
#define SimSlot3  Serial1

#include "WiFi.h"
#include "ArduinoJson.h"
#include "connection.h"
#include "WebSocketsClient.h"
#include "SocketIOclient.h"
#include  "maintasks/sockethelper.h"

#endif













  /*
  //todo : check onsolicited messaeges and process them
  //step 1 : check if datasubs an unsolicited message
  if(data.indexOf(UNSOL_MSG) > 0){
      data = data.substring(data.indexOf(UNSOL_MSG));
      //step 2: if yes, read the index 
      char rd[10];
      memset (rd,'\0',sizeof rd);
      sprintf(rd,"%s%c","AT+CMGR=",'t'); // change t to index
      SimSlot1.println(rd);
      //step 3: read teh sms on that index
      delay(1000);
      datasub = "aaaa ";
      if ( SimSlot1.available() >= 1){
        while(SimSlot1.available()){
          char c = SimSlot1.read();
          datasub+=c;
          delayMicroseconds(1050); 
          }
        }
      //step 4: delete the sms
      memset (rd,'\0',sizeof rd);
      sprintf(rd,"%s%c","AT+CMGD=",'t'); // change t to index
      SimSlot1.println(rd);
      //step 5: save the message in MMC
      

      //step x: push the message to the cloud
      // replySocket(datasub);
      USE_SERIAL.println(datasub);
    }*/