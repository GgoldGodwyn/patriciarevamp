#ifdef ARDUINO_ESP32_DEV
#include <debugger.h>
#include <helpers.h>
#include <connection.h>
#include "uartResponse.h"
#include <Arduino.h>

//Member variable declarations
bool socketConnected;
bool connectioWasAlive = true;
bool expectMessage;
Debugger *ptrDebugger;    //Pointer to debugger task
SocketIoClient *ptrSocketio;
WiFiMulti espWifiMulti;

//group of error codes as it exists on the server end
enum Code {
    DeserializeError,
    SerializeError,
    ReadSMSError,
    CheckBalError,
    UnkownError
};

/************************************************
*     Function Definations                      *
************************************************/
//desolve every blocking previoius connection and reconnect
boolean connectWifi(){
  #if (DEBUG == 1)
  USE_SERIAL.print("wifi state:  ");
  USE_SERIAL.println(WiFi.getMode());
  #endif
  WiFi.disconnect(true);
  delay(1000);
  #if (DEBUG == 1)
  USE_SERIAL.print("wifi state:  ");
  USE_SERIAL.println(WiFi.getMode());
  #endif
  int i=0;
  //now connect to hotspot
       
  while (espWifiMulti.run() != WL_CONNECTED)
  {
      delay(500);
        #if (DEBUG == 1)
        USE_SERIAL.print(i);
        #endif
        i++;
        if(i>10){
        return false;
          }
  }
  return true;
}

/************************************************
*     Function Definations                      *
************************************************/
//Initialize Wifi and connect to an access point
void initWiFi() {
  //Setup Wifi
  espWifiMulti.addAP(WIFI_PRIMARY_SSID , WIFI_PRIMARY_PASS);
  espWifiMulti.addAP(WIFI_SECONDARY_SSID , WIFI_SECONDARY_PASS);
  espWifiMulti.addAP( WIFI_TERTIARY_SSID , WIFI_TERTIARY_PASS);
  //WifiMulti.addAP(WIFI_TERTIARY_APT, WIFI_TERTIARY_SSID);
  
  while(connectWifi()==false){
    interval = noNetwork;
  }
  #if (DEBUG == 1)
  USE_SERIAL.println("connnectd to Wifi");
  USE_SERIAL.print("Connected to ");
  USE_SERIAL.println(WiFi.SSID());             // Tell us what network we're connected to
  USE_SERIAL.print("IP address:\t");
  USE_SERIAL.print(WiFi.localIP());
  #endif
}

//Monitor Wifi connection status
void monitorWiFi() {
  if (espWifiMulti.run() != WL_CONNECTED)
  {
      interval = noNetwork;
    if (connectioWasAlive == true)
    {
      connectioWasAlive = false;
  #if (DEBUG == 1)
      USE_SERIAL.print("Looking for WiFi ");
  #endif
    }
  #if (DEBUG == 1)
    USE_SERIAL.print(".");
  #endif
    delay(500);
  }
  else if (connectioWasAlive == false)
  {
    connectioWasAlive = true;
  #if (DEBUG == 1)
    USE_SERIAL.printf(" connected to %s\n", WiFi.SSID().c_str());
  #endif
  }
}

//websocket.io connected event handler
void connect(const char *payload, size_t length) {
  // #if (DEBUG == 1)
  //USE_SERIAL.printf("opened connection:");
  USE_SERIAL.println(payload);
  // #endif
  socketConnected = true;
}

//websocket.io disconnected event handler
void disconnected(const char *payload, size_t length) {
  #if (DEBUG == 1)
  //USE_SERIAL.printf("connection closed:");
  #endif
  socketConnected = false;
  ptrSocketio->beginSSL(SERVER_HOST, SERVER_PORT, SERVER_URL, SSL_CERT);
  interval = 500; 
}

//set the socketio varaible pointer for this file
void setSocketioPtr(SocketIoClient *_ptrSocketio) {
  ptrSocketio = _ptrSocketio;
}

//Allows AppManager to loop the socket manually
void loopSocket()
 {
  // if (millis()- lastpingtime > PING_CHECK_INTERVAL) {
  //   if (!Ping.ping(SERVER_HOST, 1)) {
  //     ptrSocketio->disconnect();
  //     return;
  //   }
  // }
  monitorWiFi();                  //ensure wifi is always reconnected if disconnected
  ptrSocketio->loop();            //loop the socketio connection to maintain heartbeat
  vTaskDelay(3);
}

//Report a hardware/firmware error to the server
void reportError(char* error, Code error_type) {
  DynamicJsonDocument doc(sizeof(error) + sizeof(error_type) + 30);
  
  doc["error"] = error;
  doc["errortype"] = error_type;

  String result;

  serializeJson(doc, result);
  #if (DEBUG == 1)
  USE_SERIAL.println(result);
  #endif
  ptrSocketio->emit("hardwareerror", result.c_str());
}

//Emit an event to the socket io server
void emit(const char *event, const char *payload) {
  ptrSocketio->emit(event, payload);
}


//Handle readsms event from the socketio server
void readsms(const char *payload, size_t length) {
  #if (DEBUG == 1)
  USE_SERIAL.printf("readsms command: %s\n", payload);
  #endif

  //TaskHandle_t uart1_sock_handle, uart2_sock_handle, uart3_sock_handle;
   DynamicJsonDocument data(1024);

// this
  DeserializationError err = deserializeJson(data, payload); 
  if (err) {
  #if (DEBUG == 1)
    USE_SERIAL.print(F("readsms command deserializeJson() failed: "));
    USE_SERIAL.println(err.c_str());
  #endif
    //reportError(strcat("readsms command Deserialization failed: ", err.c_str()), DeserializeError);
    return;
  }

  //data-object items extraction
  int slot_id = data["slot_id"]; 
  int command = data["command"];

  String readsmsstr = "";
 
  
  #if (DEBUG == 1)
  USE_SERIAL.print("cmd :");
  USE_SERIAL.println(command);
  USE_SERIAL.print("slot_id :");
  USE_SERIAL.println(slot_id);
  #endif

  readsmsstr += READSMS;
  readsmsstr += " ";
  readsmsstr += command;
  readsmsstr += " ";


  if (slot_id > 0 && slot_id < 6) {
  readsmsstr += slot_id;
    while(Serial.availableForWrite() < 1) {} 
    Serial.print(readsmsstr); 
    Serial.flush();
  }
  else if (slot_id > 5 && slot_id < 11) {
  readsmsstr += (slot_id - 5);
    while(Serial1.availableForWrite() < 1) {}
    Serial1.print(readsmsstr);
    Serial1.flush(); 
  }
  else if (slot_id > 10 && slot_id < 16) {
  readsmsstr += (slot_id - 10);
    while(Serial2.availableForWrite() < 1) {}
    Serial2.print(readsmsstr);
    Serial2.flush(); 
  }

  if(command == 1){
    ongoingTask++;
  }
  else{
    if(ongoingTask>0)
        ongoingTask-- ;
  }
  // Serial.print("ongoingTask : ");
  // Serial.println(ongoingTask);
}

void read_sms_pend(const char *payload, size_t lenth){

  DynamicJsonDocument data(1024);
  deserializeJson(data, payload);
  if ( data.isNull() ) {}
  else{
    // create an empty string to rebuild during loop
  String P_data = " "; 
    for(int i = 1; i<16; i++){ // loop through 1-15 simslot
      P_data = "data";
      P_data += i;
      if (data.containsKey(P_data)) { // find key
      JsonObject root = data[P_data];
      #if (DEBUG == 1)
      USE_SERIAL.print(" this prints : ");
      #endif
      char valU[50];
      serializeJson(root,valU,sizeof valU);
      #if (DEBUG == 1)
      USE_SERIAL.print("valU  :  ");
      USE_SERIAL.println(valU);
      #endif

      readsms(valU,sizeof valU);  //call read sms function
      }
    }
  }
}

// handle incoming messages
void message(const char *payload, size_t length){
   #if (DEBUG == 1)
  USE_SERIAL.println(payload);
   #endif
   if(strstr(payload, "Connected")){
     interval = connectedToServer;
   }
}

//handle check balance event from the socketio server
void checkbal(const char *payload, size_t length) {
  #if (DEBUG == 1)
  USE_SERIAL.printf("checkbal command: %s\n", payload);
  #endif

//   //TaskHandle_t uart1_sock_handle, uart2_sock_handle, uart3_sock_handle;
   DynamicJsonDocument data(1024);

  DeserializationError err = deserializeJson(data, payload); 
  if (err) {
  #if (DEBUG == 1)
    USE_SERIAL.print(F("checkbal command deserializeJson() failed: "));
    USE_SERIAL.println(err.c_str());
  #endif
    //reportError(strcat("checkbal command Deserialization failed: ", err.c_str()), DeserializeError);
    return;
  }

  //data-object items extraction
  int slot_id = data["slot_id"]; 
  int command = data["command"];

  String checkbalstr = "";

  
  #if (DEBUG == 1)
  USE_SERIAL.print("cmd :");
  USE_SERIAL.println(command);
  USE_SERIAL.print("slot_id :");
  USE_SERIAL.println(slot_id);
  #endif

  checkbalstr += CHECKBAL;
  checkbalstr += " ";
  checkbalstr += command;
  checkbalstr += " ";


  if (slot_id > 0 && slot_id < 6) {
  checkbalstr += slot_id;
    while(Serial.availableForWrite() < 1) {} 
    Serial.print(checkbalstr); 
    Serial.flush();
  }
  else if (slot_id > 5 && slot_id < 11) {
  checkbalstr += (slot_id - 5);
    while(Serial1.availableForWrite() < 1) {}
    Serial1.print(checkbalstr);
    Serial1.flush(); 
  }
  else if (slot_id > 10 && slot_id < 16) {
  checkbalstr += (slot_id - 10);
    while(Serial2.availableForWrite() < 1) {}
    Serial2.print(checkbalstr);
    Serial2.flush(); 
  }
}

//handle dial USSD event from the socketio server
void execute_ussd(const char *payload, size_t length) {
  #if (DEBUG == 1)
  USE_SERIAL.printf("dialUSSD command: %s\n", payload);
  #endif

  //TaskHandle_t uart1_sock_handle, uart2_sock_handle, uart3_sock_handle;
   DynamicJsonDocument data(1024);

// this
  DeserializationError err = deserializeJson(data, payload); 
  if (err) {
  #if (DEBUG == 1)
    USE_SERIAL.print(F("checkbal command deserializeJson() failed: "));
    USE_SERIAL.println(err.c_str());
  #endif
    //reportError(strcat("checkbal command Deserialization failed: ", err.c_str()), DeserializeError);
    return;
  }

  //data-object items extraction
  int slot_id = data["slot_id"]; 
  String code = data["code"];
  int has_options = data["has_options"];
  String theOptions = "";
    int loop = data["loop"];
  if(has_options>0){
    USE_SERIAL.print("loop");
    USE_SERIAL.println(loop);
    USE_SERIAL.println("it has more options");
    for(int lup=0; lup<loop; lup++){
    String valU=data["options"][lup];
    theOptions += valU;
    theOptions += " ";
  }

    USE_SERIAL.println(theOptions);
  }
  else{
    loop = 0;
    USE_SERIAL.println("it has no options");
  }


  String checkbalstr = "";

  
  #if (DEBUG == 1)
  USE_SERIAL.print("slot_id :");
  USE_SERIAL.println(slot_id);
  USE_SERIAL.print("code :");
  USE_SERIAL.println(code);
  USE_SERIAL.print("has_options :");
  USE_SERIAL.println(has_options);
  #endif

  checkbalstr += DAILUSSD;
  checkbalstr += " ";
  checkbalstr += "1"; // remove this if statis is later provided
  checkbalstr += " ";
  checkbalstr += slot_id;
  checkbalstr += " ";
  checkbalstr += code;
  checkbalstr += " ";
  checkbalstr += has_options;
  checkbalstr += " ";
  checkbalstr += loop;
  checkbalstr += " ";
  checkbalstr += theOptions;
  checkbalstr += " ";
  #if (DEBUG == 1)
  USE_SERIAL.print("checkbalstr : ");
  USE_SERIAL.println(checkbalstr);
  #endif



  if (slot_id > 0 && slot_id < 6) {
  checkbalstr += slot_id;
    while(Serial.availableForWrite() < 1) {} 
    Serial.print(checkbalstr); 
    Serial.flush();
  }
  else if (slot_id > 5 && slot_id < 11) {
  checkbalstr += (slot_id - 5);
    while(Serial1.availableForWrite() < 1) {}
   Serial1.print(checkbalstr);
    Serial1.flush(); 
  }
  else if (slot_id > 10 && slot_id < 16) {
  checkbalstr += (slot_id - 10);
    while(Serial2.availableForWrite() < 1) {}
    Serial2.print(checkbalstr);
    Serial2.flush(); 
  }
}

#endif