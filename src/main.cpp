
/****   Include Libraries    ****/
#include <Arduino.h>
#include "macros.h"

#define dummyMessage 0
#define hasMMC 0 // very important

#if (hasMMC == 1)
#include "advance/webServerConfig.h"
#include "advance/Card.h"
#include "advance/mySql.h"
#include "advance/server.h"
#endif

void blinkerLED( void * pvParameters );
//Declare Objects
SocketIoClient websocket;
WebSocketsClient websck;


void blinkerLED( void * pvParameters );
void wsEmit(const char *payload);

TaskHandle_t SocketioHandle;

#include "maintasks/uartResponse.h"

void setup() {
  
  pinMode(LED_BLINKER, OUTPUT);
  pinMode(34, OUTPUT);

  Serial.begin(HARD_BAUD);
  
  Serial1.begin(HARD_BAUD, SERIAL_8N1, RXD1, TXD1);
  
  Serial2.begin(HARD_BAUD, SERIAL_8N1, RXD2, TXD2);

  Serial1.println("Serial1");
  Serial2.println("Serial2");


  // to save a messege use createAndWriteFile(String buff); where buff is the message from the promini/sim module
  
//replySocket("bbbb history_id : 12 AT+CUSD=1,4OK+CUSD: 0, Your data balance:Bonus: 20MB expires 02/11/2021 23:59:59,");
  xTaskCreatePinnedToCore(  //PIN socketIO to core 1
    blinkerLED,         //Point to the tasks function
    "blinkerTask",       //name indentifier for the task
    20480,                    //stack size of the task
    NULL,                     //parameters of the task
    2,                        //priority of the task
    &SocketioHandle,         //  Hanlder variable for the task
    1
    );

      SimSlot1.flush();
      SimSlot2.flush();
      SimSlot3.flush();
      
      for(int t = 0; t<3; t++){
        SimSlot1.println("ATE0");
        SimSlot2.println("ATE0");
        SimSlot3.println("ATE0");
        delay(500);
        SimSlot1.println("AT+CMGF=1");
        SimSlot2.println("AT+CMGF=1");
        SimSlot3.println("AT+CMGF=1");
        delay(500);
      // SimSlot2.println("ATD+2348161111269;");
      // SimSlot1.println("ATD+2348161111269;");
      // SimSlot3.println("ATD+2348161111269;");

      
        Serial.print("SimSlot1 : ");
        while(SimSlot1.available()){
          Serial.print(SimSlot1.read());
          delayMicroseconds(2000);
        }
        Serial.print("\nSimSlot2 : ");
        while(SimSlot2.available()){
          Serial.print(SimSlot2.read());
          delayMicroseconds(2000);
        }
        Serial.print("\nSimSlot3 : ");
        while(SimSlot3.available()){
          Serial.print(SimSlot3.read());
          delayMicroseconds(2000);
        }
        Serial.print("\n");
      }

// while(1){
//   uart2getResponse();
// }

      

delay(2000);
initWiFi();

#if (hasMMC == 1)
  initCard();
setupServer(); // start async web server inplementing local storage within esp
  setupDateTime();
// checkForESPupdate();
#endif
    
  //Register Socketio events to handler functions and start it
   websocket.on("connect", connect);
   websocket.on("disconnected", disconnected);
   websocket.on("read_sms", readsms);
   websocket.on("checkbal", checkbal);
   websocket.on("message", message);
   websocket.on("read_sms_pend",read_sms_pend);
   websocket.on("execute_ussd",execute_ussd);
   websocket.on("logger",Logger);




   setSocketioPtr(&websocket);
   setWebsocketPtr(&websck);
    websocket.beginSSL(SERVER_HOST,SERVER_PORT,SERVER_URL);

    

}


//Program will run mainly here
void loop() {
  #if (hasMMC == 1)
  ServerPinger();
  #endif
  // GO! Run the scheduler - it never returns.
  // scheduler.runTasks();
   loopSocket();
   processProminiData();
  
  if(SimSlot3.available() >= 1){
  #if (DEBUG == 1)
    USE_SERIAL.print(F("REACH HERE: Serial 2"));
  #endif
      Serial.println(" pingVerified is = 2");
    SimSlot3getResponse();
  }
  if(SimSlot2.available() >= 1){
  #if (DEBUG == 1)
    USE_SERIAL.print(F("REACH HERE: Serial 1"));
  #endif
      Serial.println(" pingVerified is = 1");
    SimSlot2getResponse();
  }
  if(SimSlot1.available() >= 1){
  #if (DEBUG == 1)
    USE_SERIAL.print(F("REACH HERE: Serial 0"));
  #endif
      Serial.println(" pingVerified is = 0");
    SimSlot1getResponse();
  }
}



void blinkerLED( void * pvParameters )
{
 for( ;; )
 {
        unsigned long currentMillis = millis();
        unsigned long currentMillis2 = currentMillis;

  // wait for 1 minute before pinging the server
  if(interval == connectedToServer){
    if ( currentMillis2 - pingTime >= 5000 ){
      if(ActiveAtTrans == 0){
      pingTime = currentMillis2;
        #if (dummyMessage == 1)
        Serial.print("Current Socket Error Number: ");
        Serial.println(errno, DEC);
        if(currentMillis2 - pingTime >= 5000){
        Serial.println("trow reply on message");
        replyMessageSocket("aaaa\n+CMGR: \"REC UNREAD\",\"test\",\"\",\"21/08/12,14:37:45+04\"\nTrans User 1 you have been credited with N1814 by the code 08080808080\n\nOK");
        }
        #endif
      }
    else if ( currentMillis2 - pingTime >= 10000 ){
      ActiveAtTrans = 0;
      pingTime = currentMillis2;
      // Serial.println("just reset to push");

    }
    }
  }

     
  if(ongoingTask>0){
    if (currentMillis - previousMillis >= interval * 3) {
      previousMillis = currentMillis;
      if (ledState == LOW) {
        ledState = HIGH;
      } else {
        ledState = LOW;
      }
      digitalWrite(LED_BLINKER, ledState);
      digitalWrite(34, !ledState);
    }
  }
  else {
    if (currentMillis - previousMillis >= (interval)) {
    previousMillis = currentMillis;
    if (ledState == LOW) {
      ledState = HIGH;
    } else {
      ledState = LOW;
    }
    digitalWrite(LED_BLINKER, ledState);
      digitalWrite(34, !ledState);
  }
  }
  /*
  every24hr, i need to check for previous 30days record and delete
  1hr in a day is (1000ms times 60sec time 60mins) = 3600000ms
  try to delete every 3600000ms/300ms
  */
 
#if (hasMMC == 1)
if(del_DB>=12000){
  setupDateTime();
  del_DB=0;
}

del_DB++;
#endif





  vTaskDelay(300);
 }
}
