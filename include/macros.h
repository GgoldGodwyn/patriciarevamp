#ifndef __MACROS_H
#define __MACROS_H


//debug
#define DEBUG             0
#define COMMENT           1
#define CODE_NOT_IN_USE   0
#define TEST_CASH_DUMP    0
#define myHTTP 0
#define myHTTPS 1
#define serverType myHTTP

#define hasMMC 1 // very important



    // extras for OTA update
    float ESP32firmware_version = 1.0;  // Set your firmware version here. Your other sketch should have a different version number.
    // String serverName = "http://192.168.43.31:8000";
    String serverName = "http://atoc.gloverapp.co";
    String otalink = "";
    int reloadFlag = 0;


// Default Definitions and LED's
#define LED_BLINKER GPIO_NUM_2
#define SLOT_RST3 GPIO_NUM_23
#define SLOT_RST2 GPIO_NUM_5
#define SLOT_RST1 GPIO_NUM_4
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


#define GSM_BAUD 9600
#define HARD_BAUD 9600
#define TERMINATION_CHAR '\n'


#define W_S_PING_INTERVAL 4000
#define WS_PONG_INTERVAL 3000
#define WS_RECONNECT_INTERVAL 3000
#define WS_DISCONNECT_TIMEOUT_COUNT 2
#define WS_PORT 81

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



String netName1 = "          ";
String netName2 = "          ";;
String netName3 = "          ";;
#define USE_SERIAL Serial
#define sim1 1
#define sim2 2
#define sim3 3

int sim1_active = 0;  
int sim2_active = 0;
int sim3_active = 0;
int slot1Taskflag = 0;
int slot2Taskflag = 0;
int slot3Taskflag = 0;


#define UNSOL_MSG "+CMTI"


#include "BluetoothSerial.h"
#include <errno.h>




#define SimSlot1  Serial // NEAR THE POWER MODULE
#define SimSlot2  Serial2
#define SimSlot3  Serial1


#include "WiFi.h"
#include <WiFiMulti.h>
#include "ArduinoJson.h"
#include "connection.h"
#include "lcdHelper.h"
#if (hasMMC == 1)
#include "Card.h"
#endif
#include <WiFiClientSecure.h>
#include "WebSocketsClient.h"
#include "SocketIOclient.h"
#include <base64.h>
#include "maintasks/Cipher.h"
#include "updater.h"


Cipher * cipher = new Cipher();
char * key = "abcdefghijklmnop";


#if(channel != MqttChannel)
    #include  "maintasks/sockethelper.h"
#else
    #define DEBUG_PRINT(...) if (DEBUG) { Serial.print(__VA_ARGS__); }
    #define DEBUG_PRINTLN(...) if (DEBUG) { Serial.println(__VA_ARGS__); }
    #define DEBUG_PRINTF(...) if (DEBUG) { Serial.printf(__VA_ARGS__); }
    //Subscribe Topics
    #define device_name "device_1"

    #define sub_read_sms "device_1/read_sms"
    #define sub_read_sms_pend "device_1/read_sms_pend"
    #define sub_dial_ussd "device_1/dail_ussd"
    #define sub_check_bal "device_1/check_bal"
    #define sub_message "device_1/message"
    //Publish Topics
    #define pub_handle_sms "glover/device-1/handle_sms"
    #define pub_gettask "glover/device_1/gettask"
    #define pub_handle_ussd_status "glover/device_1/handle_ussd_status"
    #define pub_error_log "glover/device_1/hardwareerror"
    #define pub_message "glover/device_1/message"
    #define pub_gsm_status ""
    #define pub_restart_gsm ""
    
    //#define USE_SERIAL SerialBT
    #define USE_SERIAL Serial

#endif



#include "processUart.h"


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