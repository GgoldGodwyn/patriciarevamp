#ifdef ARDUINO_ESP32_DEV

#include <config.h>
// #include "WString.h"
#include <ESP32Ping.h>
#include <ESPDateTime.h>
#define PING_CHECK_INTERVAL 300

unsigned long lastpingtime;
int pingVerified=0;

void setupDateTime() {  
  DateTime.setServer("pool.ntp.org");
  DateTime.setTimeZone("UTC");//"CST-726"-726
  DateTime.begin();
  if (!DateTime.isTimeValid()) {
    #if (DEBUG == 1)
      Serial.println("Failed to get time from server.");
    #endif
  } else {
    #if (DEBUG == 1)
    Serial.printf("Date Now is %s\n", DateTime.toISOString().c_str());
    Serial.printf("Timestamp is %ld\n", DateTime.now());
    #endif
  }
  #if (DEBUG == 1)
  // Serial.printf("%04d/%02d/%02d %02d:%02d:%02d %ld (%s)\n", p.getYear(),
  //               p.getMonth(), p.getMonthDay(), p.getHours(), p.getMinutes(),
  //               p.getSeconds(), p.getTime(), p.getTimeZone());
  
  // Serial.print("EXPIRED date : ");
  // Serial.println(expiredDate);
  // deleteOldMessage("21/08/12");
  Serial.print(":::::::::: : ");
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
      if (ws_domain_resolved) {
        sprintf(msg, "%s%s%lld%ld%s%s",device_name,"**",current_time,current_millis,"***",data.c_str());
        wsEmit(msg);
      }
      memset(msg, '\0', strlen(msg));
      datasub = base64::encode(data);

  #if (DEBUG == 1)
      USE_SERIAL.print("the Encoded data : ");
      USE_SERIAL.println(datasub);
  #endif

      DynamicJsonDocument doc(datasub.length() + 20);
      sprintf(msg, "%lld%ld", current_time,current_millis);
      doc["message"] = datasub;
      doc["uuid"] = msg;

      data = "";


      serializeJson(doc, data);

      
      #if (DEBUG == 1)
      USE_SERIAL.print("i got to attempt sending on mqtt :: ");
      Serial.println(data);
      USE_SERIAL.print("  datasub as  :: ");
      Serial.println(datasub);
      #endif

   
    if (!mqtt_client.connected()) {     
    if (mqtt_client.connect(device_name,mqtt_username,mqtt_password)) {
      Serial.println("just connected ins");
      //Once connected, publish an announcement...
      // mqtt_client.publish("device_1/message", "hello world");
      // ... and resubscribe
      
      mqtt_client.subscribe(sub_read_sms);
      // mqtt_client.subscribe(sub_read_sms_pend);
      mqtt_client.subscribe(sub_dial_ussd);
      // mqtt_client.subscribe(sub_check_bal);
      mqtt_client.subscribe(sub_message);
    } else {
      Serial.print("failed to connect ins");
      Serial.print(mqtt_client.state());
    }
    }
    else{
      // Serial.println(" already connected");
    }
   
      //wsEmit(data.c_str());
      mqtt_client.publish(pub_handle_sms, data.c_str());
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
      Serial.println("datasub : ");
      Serial.println(data);
      Serial.println();
      while(1);
        wsEmit(data.c_str());
      ////////////////////////
        mqtt_client.publish(pub_handle_ussd_status, data.c_str());
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
  replySocket("aaaa hello world 2 africa 1");
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
      // Serial.println(" pingVerified is = 0");
		}
    else{
      pingVerified=1;
      // Serial.println(" pingVerified is = 1");
    }
  }
  #endif


  if(Serial2.available() >= 1){
  #if (DEBUG == 1)
    USE_SERIAL.print(F("REACH HERE: Serial 2"));
  #endif
      // Serial.println(" pingVerified is = 1");
    uart2getResponse();
  }
  if(Serial1.available() >= 1){
  #if (DEBUG == 1)
    USE_SERIAL.print(F("REACH HERE: Serial 1"));
  #endif
      // Serial.println(" pingVerified is = 1");
    uart1getResponse();
  }
  if(Serial.available() >= 1){
  #if (DEBUG == 1)
    USE_SERIAL.print(F("REACH HERE: Serial 0"));
  #endif
      // Serial.println(" pingVerified is = 1");
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
   Serial.println(datasub.indexOf('+CMTI'));
  #endif
    }

    // +CMTI: "SM",32 FFGFGHGFDYFYTFRD
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




void keepComms(){	
  #if(0)
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
      // Serial.println(" pingVerified is = 0");
		}
    else{
      pingVerified=1;
      
      // Serial.println(" pingVerified is = 1");
    }
  }
  #else
     
    // device_name += String(random(0xffff), HEX);
    if (!mqtt_client.connected()) {     
    if (mqtt_client.connect(device_name,mqtt_username,mqtt_password)) {
      Serial.println("just connected");
      //Once connected, publish an announcement...
      // mqtt_client.publish("device_1/message", "hello world");
      // ... and resubscribe
      
      mqtt_client.subscribe(sub_read_sms);
      // mqtt_client.subscribe(sub_read_sms_pend);
      mqtt_client.subscribe(sub_dial_ussd);
      // mqtt_client.subscribe(sub_check_bal);
      mqtt_client.subscribe(sub_message);
    } else {
      Serial.print("failed to connect");
      Serial.print(mqtt_client.state());
    }
    }
    else{
      // Serial.println(" already connected");
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

  //read all at once
  String datasub = "aaaa slot1Task";
  SimSlot1.println("AT+CMGA");
  delay(500);
  if ( SimSlot1.available() >= 1){
  while(SimSlot1.available()){
    char c = SimSlot1.read();
    datasub.concat(c);
    delayMicroseconds(1050); 
    }
  }
  
  #if (hasMMC == 1)
  createAndWriteFile(datasub);
  SDlogger(datasub);// save all to mmc first
  #endif
  // else load it in spiff

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

  //read all at once
  String datasub = "aaaa ";
  SimSlot2.println("AT+CMGA");
  delay(500);
  if ( SimSlot2.available() >= 1){
  while(SimSlot2.available()){
    char c = SimSlot2.read();
    datasub.concat(c);
    delayMicroseconds(1050); 
    }
  }
  
  #if (hasMMC == 1)
  createAndWriteFile(datasub);
  SDlogger(datasub);// save all to mmc first
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
  String datasub = "aaaa ";
  SimSlot3.println("AT+CMGA");
  delay(500);
  if ( SimSlot3.available() >= 1){
  while(SimSlot3.available()){
    char c = SimSlot3.read();
    datasub.concat(c);
    delayMicroseconds(1050); 
    }
  }
  
  #if (hasMMC == 1)
  createAndWriteFile(datasub);
  SDlogger(datasub);// save all to mmc first
  #endif

  replySocket(datasub);
}


#endif