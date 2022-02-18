#include "Arduino.h"


/*
AT+CMGF=1
To format SMS as a TEXT message
AT+CMGS="+yyyyy" <Enter> //+yyyyy is the number of the GSM
> Your SMS text message here <Ctrl-Z>
When a new SMS is received by the GSM modem, the DTE will receive the following ..
+CMTI: "SM",3
Notification sent to the computer. Location 3 in SIM memory
AT+CMGR=3 <Enter> 
*/


// #define SimSlot1 Serial
// #define DebugPrint Serial.println
// #define SimSlot2 Serial1
// #define SimSlot3 Serial2


void readSMS(uint8_t index, int mySerial)
{
  char readString[20];
  memset(readString,'\0',sizeof readString);
  sprintf(readString,"%s%i","AT+CMGR=",index);
  
  SimSlot1.Serial_port = mySerial;
  if(mySerial==1){
      SimSlot1.SendAT();
  }
  
//   SimSlot1.SendAT(readString);

//   readReply(5000, MESSAGELINES);
//   return readString;
}














void printSerial(void){
    readSMS(1,sim1);
    readSMS(1,sim2);
    readSMS(1,sim3);
}


int checkAllSerial(){
/*
    if(SimSlot1.available()){
      Serial.println("returning 1");
      return 1;
    }
    if(SimSlot2.available()){
      
      Serial.println("returning 2");
      return 2;
    }
    if(SimSlot1.available()){
      Serial.println("returning 3");
      return 3;
    }*/
    
    
    return 1;
}

void read(int simSlot){
    int tr =0;
    /*switch (simSlot)
    {
    case 1:
         read all buffer  
        delay(200);
        while (SimSlot1.available())
        {
            buffer[tr] = SimSlot1.read();
        }        
        break;
    default:
        break;
    }*/
}

/*
to read incoming sms's
*/
boolean buffer_contains(const char* find){

return true;
}


int checkDataType(int simSlot){
    switch (simSlot)
    {
    case 1:
        /* capture message */
        memset(SimSlot1.buffer,'\0',sizeof SimSlot1.buffer);
        read(simSlot);
        //check if data is a message
        if(buffer_contains("+CMTI\r\n")){
            #if CODE_NOT_IN_USE
            DebugPrint("a message just entered");
            #endif
        }
        else{
            #if CODE_NOT_IN_USE
            DebugPrint("something else popped in");
            #endif
        }
        break;
    default:
        break;
    }
return 1;
}