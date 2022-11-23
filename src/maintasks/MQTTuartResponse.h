#ifdef ARDUINO_ESP32_DEV

#include <config.h>
// #include "WString.h"
#include <ESP32Ping.h>
#include <ESPDateTime.h>
#include "mbedtls/pk.h"
#define PING_CHECK_INTERVAL 300

unsigned long lastpingtime;
int pingVerified=0;

void setupDateTime() {  
  DateTime.setServer("pool.ntp.org");
  DateTime.setTimeZone("UTC");//"CST-726"-726
  DateTime.begin();
  if (!DateTime.isTimeValid()) {
    #if (DEBUG == 1)
      USE_SERIAL.println("Failed to get time from server.");
    #endif
  } else {
    #if (DEBUG == 1)
    USE_SERIAL.printf("Date Now is %s\n", DateTime.toISOString().c_str());
    USE_SERIAL.printf("Timestamp is %ld\n", DateTime.now());
    #endif
  }
  #if (DEBUG == 1)
  // Serial.printf("%04d/%02d/%02d %02d:%02d:%02d %ld (%s)\n", p.getYear(),
  //               p.getMonth(), p.getMonthDay(), p.getHours(), p.getMinutes(),
  //               p.getSeconds(), p.getTime(), p.getTimeZone());
  
  // Serial.print("EXPIRED date : ");
  // Serial.println(expiredDate);
  // deleteOldMessage("21/08/12");
  USE_SERIAL.print(":::::::::: : ");
  #endif
  
}

bool wsEmit(const char *payload) {
  size_t len = strlen(payload)+1;
  char *text = new char[len];
  strcpy(text, payload);
  return client.sendTXT(text);
}

bool wsEmit(char *payload) {
  return client.sendTXT(payload);
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
      mqtt_client.publish(pub_message, datasub.c_str());
}
unsigned long getTime() {
  time_t now;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    //Serial.println("Failed to obtain time");
    return(0);
  }
  time(&now);
  return now;
}

void replySocket(String data){

  #if (DEBUG == 1)
  USE_SERIAL.print("processing replySocket command ");
  USE_SERIAL.println(data);
  #endif

  if(data.indexOf(READSMS) > 0){
    data = data.substring(data.indexOf(READSMS)); // clears junks before READSMS 
  }
  else if(data.indexOf(DAILUSSD) > 0){
    data = data.substring(data.indexOf(DAILUSSD));
  }
    String datasub = data.substring(0, CMD_LEN);
    //const char *response = data.substring(CMD_LEN + 2, data.length()-1).c_str();


  #if (DEBUG == 1)
    USE_SERIAL.println("replySocket command process");
  #endif
    int inputStringLength = (data.length()-1-CMD_LEN+2);
    char inputString[inputStringLength];
    data.substring(CMD_LEN + 2, data.length()-1).toCharArray(inputString, inputStringLength);
    
  #if (DEBUG == 1)
  USE_SERIAL.println(inputString);
  #endif
    unsigned long long current_time = DateTime.now();
    unsigned long current_millis = millis();
    char msg[500];
    if (datasub == READSMS) {

      #if (hasMMC == 1)
      SDlogger(data);// save all to mmc first
      #endif

      if (ws_domain_resolved) {
        sprintf(msg, "%s%s%lld%ld%s%s",device_name,"**",current_time,current_millis,"***",data.c_str());
        wsEmit(msg);
      }
      memset(msg, '\0', strlen(msg));
      
      
      //before is 
      // datasub = base64::encode(data);

      // get epoch time      
      unsigned long epochTime = getTime();
      #if(DEBUG == 1)
        Serial.print("Epoch Time: ");
        Serial.println(epochTime);
      #endif



      datasub = base64::encode(data);
      #if (DEBUG == 1)
          USE_SERIAL.print("the Encoded data : ");
          USE_SERIAL.println(datasub);
      #endif

      DynamicJsonDocument doc(datasub.length() + 200);
      doc["message"] = datasub;
      
      data = epochTime;
      data +='.';
      data += datasub;

      //hmac sha256
      byte hmacResult[32];
      mbedtls_md_context_t ctx;
      mbedtls_md_type_t md_type = MBEDTLS_MD_SHA256;
      const size_t payloadLength = strlen(data.c_str());
      const size_t keyLength = strlen(secret_token.c_str());            
      mbedtls_md_init(&ctx);
      mbedtls_md_setup(&ctx, mbedtls_md_info_from_type(md_type), 1);
      mbedtls_md_hmac_starts(&ctx, (const unsigned char *) secret_token.c_str(), keyLength);
      mbedtls_md_hmac_update(&ctx, (const unsigned char *) data.c_str(), payloadLength);
      mbedtls_md_hmac_finish(&ctx, hmacResult);
      mbedtls_md_free(&ctx);
      #if(DEBUG == 1)
      Serial.print("hmac : ");
      #endif
      for(int i= 0; i< sizeof(hmacResult); i++){
        char str[3];
        sprintf(str, "%02x", (int)hmacResult[i]);
        #if(DEBUG == 1)
        Serial.print(str);
        #endif
    }
  

      // sprintf(msg, "%lld%ld", current_time,current_millis);
//hmacResult
      // DynamicJsonDocument doc(sizeof(hmacResult) + 20);
      // sprintf(msg, "%lld%ld", current_time,current_millis);
      String fingerP = (String)epochTime;
      fingerP+='.';
      char str[3];
      for(int i= 0; i< sizeof(hmacResult); i++){
        sprintf(str, "%02x", (int)hmacResult[i]);
        #if(DEBUG == 1)
        Serial.print(str);
        #endif
        fingerP+=str;
        }
        #if(DEBUG == 1)
        Serial.println();
        #endif


      doc["fingerprint"] = fingerP;


      data = "";


      serializeJson(doc, data);

      
      #if (DEBUG == 1)
      USE_SERIAL.print("i got to attempt sending on mqtt :: ");
      USE_SERIAL.println(data);
      USE_SERIAL.print("  datasub as  :: ");
      USE_SERIAL.println(datasub);
      #endif

   
    if (!mqtt_client.connected()) {    
      newInfo = 1; 
    if (mqtt_client.connect(device_name.c_str(),mqtt_username.c_str(),mqtt_password.c_str())) {
      mqtt = 1;
      #if(DEBUG == 1)
      USE_SERIAL.println("just connected ins");
      #endif
      //Once connected, publish an announcement...
      // mqtt_client.publish("device_1/message", "hello world");
      // ... and resubscribe
      
      mqtt_client.subscribe(sub_read_sms);
      // mqtt_client.subscribe(sub_read_sms_pend);
      mqtt_client.subscribe(sub_dial_ussd);
      // mqtt_client.subscribe(sub_check_bal);
      mqtt_client.subscribe(sub_message);
    } else {
      mqtt = 0;
      #if(DEBUG == 1)
      USE_SERIAL.print("failed to connect ins");
      USE_SERIAL.print(mqtt_client.state());
      #endif
    }
    }
    else{
      #if(DEBUG == 1)
      USE_SERIAL.println(" already connected");
      #endif
    }
   
      //wsEmit(data.c_str());
      mqtt_client.publish(pub_handle_sms, data.c_str());
      #if(DEBUG == 1)
      Serial.println(data);
      #endif
      // mqtt_client.publish(pub_handle_sms, "pat-i",true);
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

    // #if (hasMMC == 1)
    // saveMessage2(data); // not yet implemented
    // #endif
    datasub = base64::encode(data);
    if (ws_domain_resolved) {
      String ws_data = "dialussd: " + data;
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
      #if(DEBUG == 1)
      USE_SERIAL.println("datasub : ");
      USE_SERIAL.println(data);
      USE_SERIAL.println();
      #endif
      while(1);
        wsEmit(data.c_str());
      ////////////////////////
        mqtt_client.publish(pub_handle_ussd_status, data.c_str());//,true
    }
    else if (datasub == GSMSTATUS) {
      //TODO process data as needed
        mqtt_client.publish(pub_gsm_status, datasub.c_str());
    }
    else if (datasub == RESTARTGSM) {
      //TODO process data as needed
        mqtt_client.publish(pub_restart_gsm, datasub.c_str());
    }
    else if (datasub == FEEDBACK) {
        //TODO: process response and send back to the GSM Module
    }
}




void uart0getResponse(){ // usb port
    // Implement your custom logic here
      String datasub="";
  #if (DEBUG == 1)
  USE_SERIAL.println("processing command"); // debug
  #endif
if ( Serial.available() >= 1 )
    {
while(Serial.available()){
    char c = Serial.read();
    datasub+=c;
    delayMicroseconds(1050);        
}
    }
  #if (DEBUG == 1)
  USE_SERIAL.println(datasub); // debug
  #endif
  replySocket(datasub);
  // vTaskDelete(NULL);// When you're done, call vTaskDelete. Don't forget this!
}

void uart1getResponse(){
  replySocket("aaaa");
  return;
    // Implement your custom logic here
      String datasub="";
  #if (DEBUG == 1)
  USE_SERIAL.println("processing command"); // debug
  #endif
if ( Serial1.available() >= 1 )
    {
while(Serial1.available()){
    char c = Serial1.read();
    datasub+=c;
    delayMicroseconds(1050); 
}
    }
  #if (DEBUG == 1)
  USE_SERIAL.println(datasub); // debug
  #endif
  replySocket(datasub);
  // vTaskDelete(NULL);// When you're done, call vTaskDelete. Don't forget this!
}


void uart2getResponse(){
  replySocket("aaaa hello world 2 africa 2");
  return;
    // Implement your custom logic here
      String datasub="";
  #if (DEBUG == 1)
  USE_SERIAL.println("processing command"); // debug
  #endif
if ( Serial2.available() >= 1 )
    {
while(Serial2.available()){
    char c = Serial2.read();
    datasub+=c;
    delayMicroseconds(1050); 
}
    }
  #if (DEBUG == 1)
  USE_SERIAL.println(datasub); // debug
  #endif
  replySocket(datasub);
  // vTaskDelete(NULL);// When you're done, call vTaskDelete. Don't forget this!
}


/*
void processProminiData(){	
  
    #if(channel != MqttChannel)
  if (millis()- lastpingtime > PING_CHECK_INTERVAL) {
    if (!Ping.ping(SERVER_HOST, 1)) {
    #if (DEBUG == 1)
      USE_SERIAL.println("failed to send..... restarting network now!!! ");
    #endif
		// ptrSocketio->disconnect();
		// WiFi.disconnect(true);
    lastpingtime = millis();
		pingVerified = 0;
      #if(DEBUG == 1)
      USE_SERIAL.println(" pingVerified is = 0");
      #ndif
		}
    else{
      pingVerified=1;
      #if(DEBUG == 1)
      USE_SERIAL.println(" pingVerified is = 1");
      #endif
    }
  }
  #endif


  if(Serial2.available() >= 1){
  #if (DEBUG == 1)
    USE_SERIAL.print(F("REACH HERE: Serial 2"));
    USE_SERIAL.println(" pingVerified is = 1");
  #endif
    uart2getResponse();
  }
  if(Serial1.available() >= 1){
  #if (DEBUG == 1)
    USE_SERIAL.print(F("REACH HERE: Serial 1"));
    USEISERIAL.println(" pingVerified is = 1");
  #endif
    uart1getResponse();
  }
  if(Serial.available() >= 1){
  #if (DEBUG == 1)
    USE_SERIAL.print(F("REACH HERE: Serial 0"));
    USE_SERIAL.println(" pingVerified is = 1");
  #endif
    uart0getResponse();
  }
  // if (redundantData !="") {
  //   replySocket("ignor");
  // }
}
*/



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
  #endif
    }


    
  if(datasub.indexOf("+CPIN: NOT READY") >= 0){
    netName1 = "  ";
    sim1_active = 0;
    newInfo = 1;
  }
  if(datasub.indexOf("+CPIN: READY") >= 0){
    sim1_active = 1;
    newInfo = 1;
  }
  
  // +CSPN: "AT&T", 1 // "SIM 3 : ",," OK"
  if(datasub.indexOf("+CSPN: \"") >= 0){
      // memset (netName1,'\0',sizeof (netName1));
    datasub = datasub.substring(datasub.indexOf("+CSPN:"));
    // sprintf(netName1,"%s",datasub.substring(datasub.indexOf('\"')+1 , datasub.indexOf('\",')-1));
    netName1 = "SIM 1 : " + datasub.substring(datasub.indexOf('\"')+1 , datasub.indexOf('\",')-1);
    sim1_active = 2;
    newInfo = 1;
  }

    
  // +COPS: 0,0, MTN Nigeria      OK
  if(datasub.indexOf("+COPS:") >= 0){
    if(datasub.length()<20){
      return;
    }
    datasub = datasub.substring(datasub.indexOf("+COPS:"));
    datasub = datasub.substring(datasub.indexOf(","));
    datasub = datasub.substring(datasub.indexOf(","));
    netName1 = "SIM 1 : " + datasub.substring(datasub.indexOf('\"')+1 , datasub.indexOf('\r')-2);
    sim1_active = 2;
    newInfo = 1;
  }

    // +CMTI: "SM",32 hi, we are testing for unsolicited messages
    /*
    aaaa +CMGR: "REC UNREAD","a2c","","21/08/12,14:37:45+04" 
    Trans User 1 you have been credited with N1814 by the code 9041315965 
    
    OK
    
    */
    /* FOR MTN
    aaaa +CMGL: 16,"REC UNREAD","777","","20/09/23,11:22:51+04"
Congrats!
You have received N50 airtime from 08036339292 via MTN Share.

OK

OR
+CMGR: "REC UNREAD","777","","20/09/23,11:22:51+04" Congrats! You have received N50 airtime from 08036339292 via MTN Share. OK
    */

   /*
   aaaa +CMGR: "REC UNREAD","AirtelERC","","21/08/12,14:37:45+04" 
   Txn Id C220505.1134.310009 of 500 NGN from 7082738974 is successful. Transferred value is 500 NGN, access fee is 0 NGN Your new balance is 500 and validity is 28/01/25. 
   
   OK
   */
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
      memset (rd,'\0',sizeof (rd));
      sprintf(rd,"%s%i","AT+CMGR=",index); 
      SimSlot1.println("AT+CMGF=1");
      delay(500);
      while(SimSlot1.available())SimSlot1.read();
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
      memset (rd,'\0',sizeof (rd));
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
  #endif
    }

  if(datasub.indexOf("+CPIN: NOT READY") >= 0){
    netName2 = "  ";
    sim2_active = 0;
    newInfo = 1;
  }
  if(datasub.indexOf("+CPIN: READY") >= 0){
    sim2_active = 1;
    newInfo = 1;
  }

  // +CSPN: "AT&Ts", 1
  if(datasub.indexOf("+CSPN: \"") >= 0){
      // memset (netName2,'\0',sizeof(netName2));
    datasub = datasub.substring(datasub.indexOf("+CSPN:"));
    // sprintf(netName2,"%s",datasub.substring(datasub.indexOf('\"')+1 , datasub.indexOf('\",')-1));
    netName2 = "SIM 2 : " + datasub.substring(datasub.indexOf('\"')+1 , datasub.indexOf('\",')-1);
    sim2_active = 2;
    newInfo = 1;
  }

  
  // +COPS: 0,0, MTN Nigeria      OK
  if(datasub.indexOf("+COPS:") >= 0){
    if(datasub.length()<20){
      return;
    }
    datasub = datasub.substring(datasub.indexOf("+COPS:"));
    datasub = datasub.substring(datasub.indexOf(","));
    datasub = datasub.substring(datasub.indexOf(","));
    netName2 = "SIM 2 : " + datasub.substring(datasub.indexOf('\"')+1 , datasub.indexOf('\r')-2);
    sim2_active = 2;
    newInfo = 1;
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
      memset (rd,'\0',sizeof (rd));
      sprintf(rd,"%s%i","AT+CMGR=",index); 
      SimSlot2.println("AT+CMGF=1");
      delay(500);
      while(SimSlot2.available())SimSlot2.read();
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
      memset (rd,'\0',sizeof (rd));
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
  #endif
    }

  if(datasub.indexOf("+CPIN: NOT READY") >= 0){
    netName3 = "  ";
    sim3_active = 0;
    newInfo = 1;
  }
  if(datasub.indexOf("+CPIN: READY") >= 0){
    sim3_active = 1;
    newInfo = 1;
  }

  
  // +CSPN: "AT&Tt", 1
  if(datasub.indexOf("+CSPN: \"") >= 0){
      // memset (netName3,'\0',sizeof (netName3));
    datasub = datasub.substring(datasub.indexOf("+CSPN:"));
    // sprintf(netName3,"%s",datasub.substring(datasub.indexOf('\"')+1 , datasub.indexOf('\",')-1));
    netName3 = "SIM 3 : " + datasub.substring(datasub.indexOf('\"')+1 , datasub.indexOf('\",')-1);
    sim3_active = 2;
    newInfo = 1;
  }

  
  // +COPS: 0,0, MTN Nigeria      OK
  if(datasub.indexOf("+COPS:") >= 0){
    if(datasub.length()<20){
      return;
    }
      
    datasub = datasub.substring(datasub.indexOf("+COPS:"));
    datasub = datasub.substring(datasub.indexOf(","));
    datasub = datasub.substring(datasub.indexOf(","));
    netName3 = "SIM 3 : " + datasub.substring(datasub.indexOf('\"')+1 , datasub.indexOf('\r')-2);
    sim3_active = 2;
    newInfo = 1;
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
      SimSlot3.println("AT+CMGF=1");
      delay(500);
      while(SimSlot3.available())SimSlot3.read();
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
      memset (rd,'\0',sizeof (rd));
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




void keepComms(){	
  #if(CODE_NOT_IN_USE)
  if (millis()- lastpingtime > PING_CHECK_INTERVAL) {
    if (!Ping.ping(SERVER_HOST, 1)) {
    #if (DEBUG == 1)
      USE_SERIAL.println("failed to send..... restarting network!!! ");
    
      replyMessageSocket("aaaa hello world");
    #endif
		// ptrSocketio->disconnect();
		// WiFi.disconnect(true);
    lastpingtime = millis();
		pingVerified = 0;
    #if(DEBUG == 1)
    USE_SERIAL.println(" pingVerified is = 0");
    #endif
		}
    else{
      pingVerified=1;
      #if(DEBUG == 1)
      USE_SERIAL.println(" pingVerified is = 1");
      #endif
    }
  }
  #else
     
    // device_name += String(random(0xffff), HEX);
    if (!mqtt_client.connected()) {   
      newInfo = 1;
    if (mqtt_client.connect(device_name.c_str(),mqtt_username.c_str(),mqtt_password.c_str())) {
      #if(DEBUG == 1)
      USE_SERIAL.println("just connected");
      #endif
      mqtt = 1;
      //Once connected, publish an announcement...
      // mqtt_client.publish("device_1/message", "hello world");
      // ... and resubscribe
      
      mqtt_client.subscribe(sub_read_sms);
      mqtt_client.subscribe("read_sms");
      mqtt_client.subscribe(sub_dial_ussd);
      // mqtt_client.subscribe(sub_check_bal);
      mqtt_client.subscribe(sub_message);
    } else {
      mqtt = 0;  
      #if(DEBUG == 1)
      USE_SERIAL.print("failed to connect");
      USE_SERIAL.print(mqtt_client.state());
      #endif
    }
    }
    else{
      #if(DEBUG == 1)
      USE_SERIAL.println(" already connected");
      #endif
    }
#endif
}








void slot1Task(){
  #if(CODE_NOT_IN_USE == 1)
  SimSlot1.println("AT+CPMS=\"SM\",\"SM\",\"SM\""); // find out if message exist in sim and memory size
  #if(DEBUG == 1)
    String resp = "+CPMS: 20,70,0,70,0,70";// dummy sim response
  #endif
  String resp = resp.substring(resp.indexOf(',') + 1);
  resp = resp.substring(0, resp.indexOf(':'));
  int usedSimSpace = resp.toInt();
  resp = resp.substring(0, resp.indexOf(','));
  int totalSimSpace = resp.toInt();
  #if(DEBUG == 1)
    USE_SERIAL.print("simSlot total storage space = ");
    USE_SERIAL.println(totalSimSpace);
    USE_SERIAL.print("simSlot used up space = ");
    USE_SERIAL.println(usedSimSpace);
  #endif
  #endif
slot1Taskflag = 0;
  //read all at once
  String datasub = "aaaa";
      // SimSlot1.println("ATE1");
      // delay(100);
      SimSlot1.println("AT+CMGDA =\"DEL READ\"");
      delay(300);
      SimSlot1.println("AT+CMGF=1");
      delay(100);
      SimSlot1.println("AT+CMGF=1");
      delay(100);
      while(SimSlot1.available()){
        char c = SimSlot1.read();
        #if(DEBUG == 1)
          Serial.print(c);
        #endif
        delayMicroseconds(1050);    
      //   SimSlot1.read();
      // delay(2);
      }
  delay(100);
  SimSlot1.println("AT+CMGL=\"ALL\"");
  delay(500);
  if ( SimSlot1.available() >= 1){
  while(SimSlot1.available()){
    char c = SimSlot1.read();
        #if(DEBUG == 1)
          Serial.print(c);
        #endif
    datasub.concat(c);
    delayMicroseconds(1050); 
    }
  }
  #if(DEBUG == 1)
  USE_SERIAL.print("all SMSs\n");
  USE_SERIAL.println(datasub);
  #endif
  
  replySocket(datasub);
}


void slot2Task(){
  #if(CODE_NOT_IN_USE == 1)
  SimSlot2.println("AT+CPMS=\"SM\",\"SM\",\"SM\""); // find out if message exist in sim and memory size
  #if(DEBUG == 1)
    String resp = "+CPMS: 20,70,0,70,0,70";// dummy sim response
  #endif
  String resp = resp.substring(resp.indexOf(',') + 1);
  resp = resp.substring(0, resp.indexOf(':'));
  int usedSimSpace = resp.toInt();
  resp = resp.substring(0, resp.indexOf(','));
  int totalSimSpace = resp.toInt();
  #if(DEBUG == 1)
    USE_SERIAL.print("simSlot total storage space = ");
    USE_SERIAL.println(totalSimSpace);
    USE_SERIAL.print("simSlot used up space = ");
    USE_SERIAL.println(usedSimSpace);
  #endif
  #endif
slot2Taskflag = 0;
  //read all at once
  String datasub = "aaaa ";
      // SimSlot2.println("ATE1");
      // delay(100);
      SimSlot2.println("AT+CMGF=1");
      delay(100);
      SimSlot2.println("AT+CMGDA =\"DEL READ\"");
      delay(300);
      SimSlot2.println("AT+CMGF=1");
      delay(100);
      while(SimSlot2.available()){
        char c = SimSlot2.read();
        #if(DEBUG == 1)
          Serial.print(c);
        #endif
        delayMicroseconds(1050);   
      // SimSlot2.read();
      // delay(2);
      }
  delay(100);
  SimSlot2.println("AT+CMGL=\"ALL\"");
  delay(500);
  if ( SimSlot2.available() >= 1){
  while(SimSlot2.available()){
    char c = SimSlot2.read();
        #if(DEBUG == 1)
          Serial.print(c);
        #endif
    datasub.concat(c);
    delayMicroseconds(1050); 
    }
  }
  #if(DEBUG == 1)
  USE_SERIAL.print("all SMSs\n");
  USE_SERIAL.println(datasub);
  #endif

  replySocket(datasub);

}


void slot3Task(){
  #if(CODE_NOT_IN_USE == 1)
  SimSlot3.println("AT+CPMS=\"SM\",\"SM\",\"SM\""); // find out if message exist in sim and memory size
  #if(DEBUG == 1)
    String resp = "+CPMS: 20,70,21,70,23,70";// dummy sim response
  #endif
  String resp = resp.substring(resp.indexOf(',') + 1);
  resp = resp.substring(0, resp.indexOf(':'));
  int usedSimSpace = resp.toInt();
  resp = resp.substring(0, resp.indexOf(','));
  int totalSimSpace = resp.toInt();
  #if(DEBUG == 1)
    USE_SERIAL.print("simSlot total storage space = ");
    USE_SERIAL.println(totalSimSpace);
    USE_SERIAL.print("simSlot used up space = ");
    USE_SERIAL.println(usedSimSpace);
  #endif
  #endif

  //read all at once
  slot3Taskflag = 0;
  String datasub = "aaaa ";
      // SimSlot3.println("ATE1");
      // delay(100);
      SimSlot3.println("AT+CMGF=1");
      delay(100);
      SimSlot3.println("AT+CMGDA =\"DEL READ\"");
      delay(300);
      SimSlot3.println("AT+CMGF=1");
      delay(100);
      while(SimSlot3.available()){
        char c = SimSlot3.read();
        #if(DEBUG == 1)
          Serial.print(c);
        #endif
        delayMicroseconds(1050);   
      // SimSlot3.read();
      // delay(2);
      }
  delay(100);
  SimSlot3.println("AT+CMGL=\"ALL\"");
  delay(500);
  if ( SimSlot3.available() >= 1){
    while(SimSlot3.available()){
      char c = SimSlot3.read();
        #if(DEBUG == 1)
          Serial.print(c);
        #endif
      datasub.concat(c);
      delayMicroseconds(1050); 
      }
  }
  
    #if (DEBUG == 1)
    USE_SERIAL.print("all SMSs\n");
    USE_SERIAL.println(datasub);
    #endif

  replySocket(datasub);
}


#endif





//replySocket("bbbb history_id : 12 AT+CUSD=1,4OK+CUSD: 0, Your data balance:Bonus: 20MB expires 02/11/2021 23:59:59,");