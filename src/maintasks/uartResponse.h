#ifdef ARDUINO_ESP32_DEV
#include <Base64.h>
#include "ESP32Ping.h"


// #include "WString.h"
// #include <ESP32Ping.h>
#define PING_CHECK_INTERVAL 300

unsigned long lastpingtime;
int pingVerified=0;
// String redundantData="";
/*
void move(String &rhs) {
    if(buffer()) {
        if(capacity() >= rhs.len()) {
            memmove(wbuffer(), rhs.buffer(), rhs.length() + 1);
            setLen(rhs.len());
	    rhs.invalidate();
            return;
        } else {
            if (!isSSO()) {
                free(wbuffer());
                setBuffer(nullptr);
            }
        }
    }
    if (rhs.isSSO()) {
        setSSO(true);
        memmove(sso.buff, rhs.sso.buff, sizeof(sso.buff));
    } else {
        setSSO(false);
        setBuffer(rhs.wbuffer());
    }
    setCapacity(rhs.capacity());
    setLen(rhs.len());
    rhs.setSSO(false);
    rhs.setCapacity(0);
    rhs.setBuffer(nullptr);
    rhs.setLen(0);
}
*/
//Emit a text to local websocket server
void wsEmit(const char *payload) {
  size_t len = strlen(payload)+1;
  char *text = new char[len];
  strcpy(text, payload);
  websck.sendTXT(text, len);
}

void replyMessageSocket(String data){
      String encodedString = base64::encode(data);
  #if (DEBUG == 1)
      USE_SERIAL.print("the Encoded data : ");
      USE_SERIAL.println(encodedString);
  #endif

      DynamicJsonDocument doc(encodedString.length() + 20);

      doc["message"] = encodedString;//"hello server";

      String datasub = "";

      serializeJson(doc, datasub);
      websocket.emit("message", datasub.c_str());
}

void replySocket(String data){
  
  #if (DEBUG == 1)
  USE_SERIAL.print("processing replySocket command ");
  USE_SERIAL.println(data);
  #endif

  if(data.indexOf(READSMS) >= 0){
    data = data.substring(data.indexOf(READSMS)); // clears junks before READSMS 
  }
  else if(data.indexOf(DAILUSSD) > 0){
    data = data.substring(data.indexOf(DAILUSSD));
  }
  
    String datasub = data.substring(0, CMD_LEN);
    //const char *response = data.substring(CMD_LEN + 2, data.length()-1).c_str();


  #if (DEBUG == 1)
    USE_SERIAL.print("data ");
    USE_SERIAL.println(data);
    USE_SERIAL.print("datasub : ");
    USE_SERIAL.println(datasub);
    USE_SERIAL.println("replySocket command process");
  #endif
    int inputStringLength = (data.length()-1-CMD_LEN+2);
    char inputString[inputStringLength];
    data.substring(CMD_LEN + 2, data.length()-1).toCharArray(inputString, inputStringLength);
    
  #if (DEBUG == 1)
  USE_SERIAL.println(inputString);
  #endif

    /*if (redundantData !="") {
      if(pingVerified==1){
        String encodedString = base64::encode(redundantData);
  #if (DEBUG == 1)
      USE_SERIAL.print("the Encoded data : ");
      USE_SERIAL.println(encodedString);
  #endif

      DynamicJsonDocument doc(encodedString.length() + 20);

      doc["message"] = encodedString;

      datasub = "";

      serializeJson(doc, datasub);
      websocket.emit(READSMS_ADDR, datasub.c_str());
      redundantData="";
      }
    }*/

    if (datasub == READSMS) {
      // events.send( saveMessage(data).c_str() ,"sms",millis());
      // events.send( saveMessage(data) ,"sms",millis());
      
      #if (hasMMC == 1)
      saveMessage(data);
      #endif
      /*if(pingVerified==0){
        redundantData+=data;
        return;
      }*/
      // int encodedLength = Base64.encodedLength(inputStringLength);
      // char encodedString[encodedLength + 1];
      // Base64.encode(encodedString, inputString, inputStringLength);
      datasub = base64::encode(data);
      if (ws_domain_resolved) {
        String ws_data = datasub + "readsms: ";
        wsEmit(ws_data.c_str());
      }

  #if (DEBUG == 1)
      USE_SERIAL.print("the Encoded data : ");
      USE_SERIAL.println(datasub);
  #endif

      DynamicJsonDocument doc(datasub.length() + 20);

      doc["message"] = datasub;

      data = "";

      serializeJson(doc, data);
      //wsEmit(data.c_str());
      websocket.emit(READSMS_ADDR, data.c_str());
    }

    else if (datasub == DAILUSSD) {
      data = data.substring(data.indexOf("history_id") + 13);
      int history_id = data.substring(0, data.indexOf(' ')).toInt();
      data = data.substring(data.indexOf(' ') + 1);

      // #if (DEBUG == 1)
      // Serial.print("history_id =");
      // Serial.println(history_id);
      // Serial.print("data : ");
      // Serial.println(data);
      // while(1);
      // #endif

    #if (hasMMC == 1)
    saveMessage2(data); // not yet implemented
    #endif
    datasub = base64::encode(data);
    datasub = base64::encode(data);
    if (ws_domain_resolved) {
      String ws_data = datasub + "dialussd: ";
      wsEmit(ws_data.c_str());
    }
  #if (DEBUG == 1)
      USE_SERIAL.print("the Encoded data : ");
      USE_SERIAL.println(datasub);
  #endif

   StaticJsonDocument<200> doc;
  //  doc["sensor"] = "gps";
  //  doc["time"] = 1351824120;

      // DynamicJsonDocument doc(encodedString.length() + 20);

      doc["response"] = datasub;
      doc["history_id"] = history_id;

      data = "";

      serializeJson(doc, data);
      Serial.println("datasub : ");
      Serial.println(data);
      Serial.println();
      while(1);
        wsEmit(data.c_str());
      ////////////////////////
        websocket.emit(DAILUSSD_ADDR, data.c_str());
    }
    else if (datasub == GSMSTATUS) {
      //TODO process data as needed
        websocket.emit(GSMSTATUS_ADDR, datasub.c_str());
    }
    else if (datasub == RESTARTGSM) {
      //TODO process data as needed
        websocket.emit(RESTARTGSM_ADDR, datasub.c_str());
    }
    else if (datasub == FEEDBACK) {
        //TODO: process response and send back to the GSM Module
    }
}



////////////simSlot 1

void SimSlot1getResponse(){
    // Implement your custom logic here
      String datasub="";
  #if (DEBUG == 1)
  USE_SERIAL.println("processing command"); // debug
  #endif
if ( SimSlot1.available() >= 1 )
    {
while(SimSlot1.available()){
    char c = SimSlot1.read();
    datasub.concat(c);
    delayMicroseconds(1050);        
}

  #if (DEBUG == 1)
  USE_SERIAL.print("datasub 1 : "); // debug
  USE_SERIAL.println(datasub); // debug
   Serial.println(datasub.indexOf('+CMTI'));
  #endif
    }


    // datasub = "+CMTI: "SM",32 ";
  // Serial.print(datasub.indexOf("+CMTI",1));
  //todo : check onsolicited messaeges and process them
  //step 1 : check if datasubs an unsolicited message
  if(datasub.indexOf("+CMTI") >= 0){
    
    datasub = datasub.substring(datasub.indexOf("+CMTI"));
  #if (DEBUG == 1)
  USE_SERIAL.print("datasub2 : "); // debug
  USE_SERIAL.println(datasub); // debug
  #endif
        uint8_t index = datasub.substring(datasub.indexOf('\",')+1 , datasub.indexOf('\",')+3).toInt();
      //step 2: if yes, read the index 
      char rd[10];
      memset (rd,'\0',sizeof rd);
      sprintf(rd,"%s%i","AT+CMGR=",index); 
      SimSlot1.println(rd);

      //step 3: read teh sms on that index
      delay(1500);
      datasub = "aaaa ";
      // while(SimSlot1.available()<1); //todo : remove this later
      if ( SimSlot1.available() >= 1){
        while(SimSlot1.available()){
          char c = SimSlot1.read();
          datasub+=c;
          delayMicroseconds(1050); 
          }
        }
        
  #if (DEBUG == 1)
  USE_SERIAL.print("datasub3 : "); // debug
  USE_SERIAL.println(datasub); // debug
  #endif
      //step 4: delete the sms
      memset (rd,'\0',sizeof rd);
      sprintf(rd,"%s%i","AT+CMGD=",index); // change t to index
      SimSlot1.println(rd);
      //step 5: save the message in MMC
      

      //step x: push the message to the cloud
      // USE_SERIAL.println(datasub);
      // datasub = "aaaa 123 testing";
  #if (DEBUG == 1)
  USE_SERIAL.print("datasub4 : "); // debug
      USE_SERIAL.println(datasub);
  USE_SERIAL.print("--------------------"); // debug
  #endif
      replySocket(datasub);
    }
  
}

///////////simSlot2

void SimSlot2getResponse(){
    // Implement your custom logic here
      String datasub="";
  #if (DEBUG == 1)
  USE_SERIAL.println("processing command"); // debug
  #endif
if ( SimSlot2.available() >= 1 )
    {
while(SimSlot2.available()){
    char c = SimSlot2.read();
    datasub.concat(c);
    delayMicroseconds(1050);        
}

  #if (DEBUG == 1)
  USE_SERIAL.print("datasub 1 : "); // debug
  USE_SERIAL.println(datasub); // debug
   Serial.println(datasub.indexOf('+CMTI'));
  #endif
    }


    // datasub = "+CMTI: "SM",32 ";
  // Serial.print(datasub.indexOf("+CMTI",1));
  //todo : check onsolicited messaeges and process them
  //step 1 : check if datasubs an unsolicited message
  if(datasub.indexOf("+CMTI") >= 0){
    
    datasub = datasub.substring(datasub.indexOf("+CMTI"));
  #if (DEBUG == 1)
  USE_SERIAL.print("datasub2 : "); // debug
  USE_SERIAL.println(datasub); // debug
  #endif
        uint8_t index = datasub.substring(datasub.indexOf('\",')+1 , datasub.indexOf('\",')+3).toInt();
      //step 2: if yes, read the index 
      char rd[10];
      memset (rd,'\0',sizeof rd);
      sprintf(rd,"%s%i","AT+CMGR=",index); 
      SimSlot2.println(rd);

      //step 3: read teh sms on that index
      delay(1500);
      datasub = "aaaa ";
      // while(SimSlot2.available()<1); //todo : remove this later
      if ( SimSlot2.available() >= 1){
        while(SimSlot2.available()){
          char c = SimSlot2.read();
          datasub+=c;
          delayMicroseconds(1050); 
          }
        }
        
  #if (DEBUG == 1)
  USE_SERIAL.print("datasub3 : "); // debug
  USE_SERIAL.println(datasub); // debug
  #endif
      //step 4: delete the sms
      memset (rd,'\0',sizeof rd);
      sprintf(rd,"%s%i","AT+CMGD=",index); // change t to index
      SimSlot2.println(rd);
      //step 5: save the message in MMC
      

      //step x: push the message to the cloud
      // USE_SERIAL.println(datasub);
      // datasub = "aaaa 123 testing";
  #if (DEBUG == 1)
  USE_SERIAL.print("datasub4 : "); // debug
      USE_SERIAL.println(datasub);
  USE_SERIAL.print("--------------------"); // debug
  #endif
      replySocket(datasub);
    }
  
}

///////////simSlot3
void SimSlot3getResponse(){
    // Implement your custom logic here
      String datasub="";
  #if (DEBUG == 1)
  USE_SERIAL.println("processing command"); // debug
  #endif
if ( SimSlot3.available() >= 1 )
    {
while(SimSlot3.available()){
    char c = SimSlot3.read();
    datasub.concat(c);
    delayMicroseconds(1050);        
}

  #if (DEBUG == 1)
  USE_SERIAL.print("datasub 1 : "); // debug
  USE_SERIAL.println(datasub); // debug
   Serial.println(datasub.indexOf('+CMTI'));
  #endif
    }


    // datasub = "+CMTI: "SM",32 ";
  // Serial.print(datasub.indexOf("+CMTI",1));
  //todo : check onsolicited messaeges and process them
  //step 1 : check if datasubs an unsolicited message
  if(datasub.indexOf("+CMTI") >= 0){
    
    datasub = datasub.substring(datasub.indexOf("+CMTI"));
  #if (DEBUG == 1)
  USE_SERIAL.print("datasub2 : "); // debug
  USE_SERIAL.println(datasub); // debug
  #endif
        uint8_t index = datasub.substring(datasub.indexOf('\",')+1 , datasub.indexOf('\",')+3).toInt();
      //step 2: if yes, read the index 
      char rd[10];
      memset (rd,'\0',sizeof rd);
      sprintf(rd,"%s%i","AT+CMGR=",index); 
      SimSlot3.println(rd);

      //step 3: read teh sms on that index
      delay(1500);
      datasub = "aaaa ";
      // while(SimSlot3.available()<1); //todo : remove this later
      if ( SimSlot3.available() >= 1){
        while(SimSlot3.available()){
          char c = SimSlot3.read();
          datasub+=c;
          delayMicroseconds(1050); 
          }
        }
        
  #if (DEBUG == 1)
  USE_SERIAL.print("datasub3 : "); // debug
  USE_SERIAL.println(datasub); // debug
  #endif
      //step 4: delete the sms
      memset (rd,'\0',sizeof rd);
      sprintf(rd,"%s%i","AT+CMGD=",index); // change t to index
      SimSlot3.println(rd);
      //step 5: save the message in MMC
      

      //step x: push the message to the cloud
      // USE_SERIAL.println(datasub);
      // datasub = "aaaa 123 testing";
  #if (DEBUG == 1)
  USE_SERIAL.print("datasub4 : "); // debug
      USE_SERIAL.println(datasub);
  USE_SERIAL.print("--------------------"); // debug
  #endif
      replySocket(datasub);
    }
  
}












void processProminiData(){	
  if (millis()- lastpingtime > PING_CHECK_INTERVAL) {
    if (!Ping.ping(SERVER_HOST, 1)) {
    #if (DEBUG == 1)
      USE_SERIAL.println("failed to send..... restarting network!!! ");
    #endif
		// ptrSocketio->disconnect();
		// WiFi.disconnect(true);
    lastpingtime = millis();
		pingVerified = 0;
      // Serial.println(" pingVerified is = 0");
		}
    else{
      pingVerified=1;
      
      // Serial.println(" pingVerified is = 1");
    }
  }
  /*
  if(Serial2.available() >= 1){
  #if (DEBUG == 1)
    USE_SERIAL.print(F("REACH HERE: Serial 2"));
  #endif
      Serial.println(" pingVerified is = 2");
    uart2getResponse();
  }
  if(Serial1.available() >= 1){
  #if (DEBUG == 1)
    USE_SERIAL.print(F("REACH HERE: Serial 1"));
  #endif
      Serial.println(" pingVerified is = 1");
    uart1getResponse();
  }
  if(Serial.available() >= 1){
  #if (DEBUG == 1)
    USE_SERIAL.print(F("REACH HERE: Serial 0"));
  #endif
      Serial.println(" pingVerified is = 0");
    uart0getResponse();
  }*/
  /*if (redundantData !="") {
    replySocket("ignor");
  }*/
}

#endif