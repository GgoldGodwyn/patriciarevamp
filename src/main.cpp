#include "Arduino.h"
#include "macros.h"




void blinkerLED( void * pvParameters );

TaskHandle_t SocketioHandle;

void setup(){
  
  pinMode(LED_BLINKER, OUTPUT);
  pinMode(34, OUTPUT);

  Serial.begin(9600);
  
  Serial1.begin(HARD_BAUD, SERIAL_8N1, RXD1, TXD1);
  
  Serial2.begin(HARD_BAUD, SERIAL_8N1, RXD2, TXD2);
  
  

  xTaskCreatePinnedToCore(  //PIN socketIO to core 1
    blinkerLED,         //Point to the tasks function
    "blinkerTask",       //name indentifier for the task
    20480,                    //stack size of the task
    NULL,                     //parameters of the task
    2,                        //priority of the task
    &SocketioHandle,         //  Hanlder variable for the task
    1
    );

// start bluetooth
//wait for connection
delay(2000);

printSerial();

      while (SimSlot2.available())    
      {
        SimSlot2.read();
      }
      while (SimSlot2.available())    
      {
        SimSlot2.read();
      }
      while (SimSlot2.available())    
      {
        SimSlot2.read();
      }
}



void loop(){
  checkDataType(checkAllSerial());

}



unsigned long pingTime = 0;
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
