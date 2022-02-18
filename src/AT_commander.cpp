

#include <Arduino.h>



// #define SimSlot1 Serial2
// #define SimSlot1_RST_PIN 6


// Read reply from gsm module
String readReply(uint32_t timeout)
{
  uint32_t previousTime = millis() , wdtCrash = millis();
  String receivedString = "";
  while (millis() - previousTime < timeout)
  {
    if(wdtCrash-millis()>200){
      vTaskDelay(20);
    //   esp_task_wdt_reset();
      wdtCrash = millis();
      }
    if (SimSlot1.available())
    {
      char inChar = (char(SimSlot1.read()));
      if ((inChar != '\n') && (inChar != '\r'))
      { /*Append data till enter character received*/
        receivedString += inChar;
      }
      // else
      // {
      //   Serial.println("notho0mg");
      //   return receivedString;
      // }
    }
  }
        // Serial.print("something :: ");
        // Serial.println(receivedString);
  return receivedString;
}
boolean checkForExptReplyinRcvdString(String recvdString, const char* exptReply)
{
  // Serial.print("exptReply : ");
  // Serial.print(exptReply);
  // Serial.print(" ::  recvdString : ");
  // Serial.println(recvdString);
  if (strstr(recvdString.c_str(), exptReply) != 0)
  {
    #if (DEBUG == 1)
        Serial.println("Got Correct Reply");
    #endif
        recvdString = "";
        return true;
    }
    else
    {
    #if (DEBUG == 1)
        Serial.println("Got ERROR");
    #endif
    recvdString = "";
    return false;
  }

}
boolean waitResponse(const char* exptReply, uint32_t timeout) {
  String inputString = "";
  inputString = readReply(timeout);
  Serial.print("sim replied : ");
  Serial.println(inputString);

//#if (DEBUG == 1)
  Serial.println(inputString);
//#endif
  boolean response = checkForExptReplyinRcvdString(inputString, exptReply);
  return response;
}
// Read reply from gsm module
String readReply(uint32_t timeout, const char* exptReply)
{
  uint32_t previousTime = millis() , wdtCrash = millis();
  String receivedString = "";
    //SimSlot1.listen();
  while (millis() - previousTime < timeout)
  {
    if(wdtCrash-millis()>200){
      vTaskDelay(20);
    //   esp_task_wdt_reset();
      wdtCrash = millis();
      }
    if (SimSlot1.available())
    {
      char inChar = (char(SimSlot1.read()));
      if ((inChar != '\n') && (inChar != '\r'))
      { /*Append data till enter character received*/
        receivedString += inChar;

        if (strstr(receivedString.c_str(), exptReply) != 0)
        return receivedString;
      }
    }
  }
  return receivedString;
}
boolean waitResponse(const char* exptReply, uint32_t timeout, int quick) {
  String inputString = "";
  inputString = readReply(timeout,exptReply);
  Serial.print("sim replied : ");
  Serial.println(inputString);

//#if (DEBUG == 1)
  Serial.println(inputString);
//#endif
  boolean response = checkForExptReplyinRcvdString(inputString, exptReply);
  return response;
}
boolean SendAT(String ATCommand, const char* exptReply, uint32_t timeout, int retry) {
  #if (DEBUG == 1)
  Serial.print("sending ");
  Serial.print(ATCommand);
  Serial.println(" to sim module");
  #endif
    int itr = 0;
    SimSlot1.flush(); /*Flush all the pending inputs in Serial buffer*/
    while (itr++ < retry) { /*wait for this many times till we get exptReply*/  
    //   esp_task_wdt_reset();
        vTaskDelay(100); /*wait for minimum before sending next AT command*/
         SimSlot1.println(ATCommand); /*Send AT command to the GSM RX pin*/
        // boolean status =  waitResponse(exptReply, timeout);
        boolean status =  waitResponse(exptReply, timeout,1);
        if (status == true)
        {
        vTaskDelay(100); 
            return true;
        }
    }
        vTaskDelay(100); 
  return false;
}