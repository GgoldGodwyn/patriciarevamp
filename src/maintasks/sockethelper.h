
#include <Arduino.h>
#include "macros.h"



//Member variable declarations
bool socketConnected;
bool connectioWasAlive = true;
bool expectMessage;

SocketIoClient *ptrSocketio;
WebSocketsClient *ptrWebSocket;


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


// Initialize WiFi
void initWiFi() {
    WiFi.mode(WIFI_STA); 
    // WiFi.mode(WIFI_MODE_APSTA);
// const char *soft_ap_ssid = "MyESP32AP";
// const char *soft_ap_password = "testpassword";
//     WiFi.softAP(soft_ap_ssid, soft_ap_password);

    WiFi.begin(WIFI_PRIMARY_SSID, WIFI_PRIMARY_PASS);
    #if DEBUG == 1
    Serial.print("Connecting to WiFi ..");
    #endif
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print('.');
        delay(1000);
    }
    Serial.println(WiFi.localIP());
}



/************************************************
*     Function Definations                      *
************************************************/
//Initialize Wifi and connect to an access point
/*void initWiFi() {
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
}*/

//Monitor Wifi connection status
unsigned long check_interval = 3000;
unsigned long check_previousMillis = 3000;
void monitorWiFi() {
  
  unsigned long my_currentMillis = millis();
  // if WiFi is down, try reconnecting every CHECK_WIFI_TIME seconds
  if ((WiFi.status() != WL_CONNECTED) && (my_currentMillis - check_previousMillis >=check_interval)) {
    interval = noNetwork;
    
    if (connectioWasAlive == true)
    {
      connectioWasAlive = false;
    #if (DEBUG == 1)
      USE_SERIAL.print("Looking for WiFi ");
    #endif
    }
  #if DEBUG == 1
    USE_SERIAL.print(millis());
    USE_SERIAL.println(" Reconnecting to WiFi...");
  #endif
    WiFi.disconnect();
    // WiFi.reconnect();
    WiFi.begin(WIFI_PRIMARY_SSID, WIFI_PRIMARY_PASS);
    check_previousMillis = my_currentMillis;
  }
  else if (connectioWasAlive == false)
  {
    connectioWasAlive = true;
  #if (DEBUG == 1)
    USE_SERIAL.printf(" connected to %s\n", WiFi.SSID().c_str());
  #endif
  
  }

  // if (espWifiMulti.run() != WL_CONNECTED)
  // {
  //     interval = noNetwork;
  //   if (connectioWasAlive == true)
  //   {
  //     connectioWasAlive = false;
  // #if (DEBUG == 1)
  //     USE_SERIAL.print("Looking for WiFi ");
  // #endif
  //   }
  // #if (DEBUG == 1)
  //   USE_SERIAL.print(".");
  // #endif
  //   delay(500);
  // }
  // else if (connectioWasAlive == false)
  // {
  //   connectioWasAlive = true;
  // #if (DEBUG == 1)
  //   USE_SERIAL.printf(" connected to %s\n", WiFi.SSID().c_str());
  // #endif
  
  // }
}

//websocket.io connected event handler
void connect(const char *payload, size_t length) {
  #if (DEBUG == 1)
  USE_SERIAL.println("opened connection:");
  USE_SERIAL.println(payload);
  #endif
  socketConnected = true;
}

//websocket.io disconnected event handler
void disconnected(const char *payload, size_t length) {
  #if (DEBUG == 1)
  USE_SERIAL.printf("connection closed:");
  #endif
  socketConnected = false;
  interval = 500; 
}

//set the socketio varaible pointer for this file
void setSocketioPtr(SocketIoClient *_ptrSocketio) {
  ptrSocketio = _ptrSocketio;
}

//set the local websocket variable pointer for this file
void setWebsocketPtr(WebSocketsClient *_ptrWebSocket) {
  ptrWebSocket = _ptrWebSocket;
}

//Allows AppManager to loop the socket manually
void loopSocket()
 {
//   for (;;) {                  //loop until scheduler terminates its execution
      ptrSocketio->loop();            //loop the socketio connection to maintain heartbeat
      if (ws_domain_resolved) {
        ptrWebSocket->loop();           //loop the local websocket connection to maintain heartbeat
      }
      monitorWiFi();                  //ensure wifi is always reconnected if disconnected
      vTaskDelay(3);
  // }
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

#if (CODE_NOT_IN_USE == 1)
//Handle readsms event from the socketio server
void readsms(const char *payload, size_t length) {
  // #if (DEBUG == 1)
  USE_SERIAL.printf("readsms command: %s\n", payload);
  // #endif

  //TaskHandle_t uart1_sock_handle, uart2_sock_handle, uart3_sock_handle;
   DynamicJsonDocument data(1024);

// this
  DeserializationError err = deserializeJson(data, payload); 
  if (err) {
  // #if (DEBUG == 1)
    USE_SERIAL.print(F("readsms command deserializeJson() failed: "));
    USE_SERIAL.println(err.c_str());
  // #endif
    //reportError(strcat("readsms command Deserialization failed: ", err.c_str()), DeserializeError);
    return;
  }

  //data-object items extraction
  int slot_id = data["slot_id"]; 
  int command = data["command"];
  
  ActiveAtTrans = command;    //this determine if ping should continue or not

  String readsmsstr = "";
 
  
  #if (DEBUG == 1)
  USE_SERIAL.print("cmd :");
  USE_SERIAL.println(command);
  USE_SERIAL.print("slot_id :");
  USE_SERIAL.println(slot_id);
  #endif

  // readsmsstr.concat(READSMS);
  // readsmsstr.concat(" ");
  // readsmsstr.concat(command);
  // readsmsstr.concat(" ");

  #if DEBUG
    USE_SERIAL.printf("%s %i %i\n", READSMS, command, slot_id);
  #endif

  if (slot_id == 1) {
    // while(Serial1.availableForWrite() < 1);

    slot1Task(); // check for sms that on sim and read them
    readRecord("/redundantSMSs.txt");
    //read from file system and replySocket(FS); 
    

    SimSlot1.flush();
  }
  else if (slot_id == 2) {
    // while(Serial.availableForWrite() < 1) ;

    slot2Task(); //todo : check for sms that on sim and read them
    readRecord("/redundantSMSs.txt");

    //read from file system and replySocket(FS); 

    
    SimSlot2.flush(); 
  }
  else if (slot_id == 3) {
    // while(Serial2.availableForWrite() < 1) ;

    slot3Task(); //todo : check for sms that on sim and read them
    readRecord("/redundantSMSs.txt");
    //read from file system and replySocket(FS); 
    
    SimSlot3.flush(); 
  }

  if(command == 1){
    ongoingTask++;
  }
  else{
    if(ongoingTask>0)
        ongoingTask-- ;
  }
}

void read_sms_pend(const char *payload, size_t lenth){
  
      #if (DEBUG == 1)
      USE_SERIAL.print("read_sms_pend reports : ");
      USE_SERIAL.println(payload);
      #endif

  DynamicJsonDocument data(1024);
  deserializeJson(data, payload);
  if ( data.isNull() ) {}
  else{
    // create an empty string to rebuild during loop
  String P_data = " "; 
    for(int i = 1; i<16; i++){ // loop through 1-15 simslot
      P_data = "data";
      P_data += i;
      if (data.containsKey(P_data)) { // find key  data2
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
      delay(300);
      }
    }
  }
}
#endif

// handle incoming messages
void message(const char *payload, size_t length){
  //  #if (DEBUG == 1)
  USE_SERIAL.print("FROM MESSAGE I GOT : ");
  USE_SERIAL.println(payload);
  //  #endif
   if(strstr(payload, "Connected")){
     interval = connectedToServer;
   }
}

// handle incoming messages
void Logger(const char *payload, size_t length){
   #if (DEBUG == 1)
  USE_SERIAL.println(payload);
   #endif
}


//handle check balance event from the socketio server
void checkbal(const char *payload, size_t length) {
  #if (DEBUG == 1)
  USE_SERIAL.printf("checkbal command: %s\n", payload);
  #endif

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
    while(Serial1.availableForWrite() < 1) {} 
    Serial1.print(checkbalstr); 
    Serial1.flush();
  }
  else if (slot_id > 5 && slot_id < 11) {
  checkbalstr += (slot_id - 5);
    while(Serial.availableForWrite() < 1) {}
    Serial.print(checkbalstr);
    Serial.flush(); 
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


 //{"slot_id":1,"history_id":40,"code":"*556#","has_options":0,"loop":0,"options":[]}
  //data-object items extraction
  int slot_id = data["slot_id"]; 
  int history_id = data["history_id"];
  String code = data["code"];
  int has_options = data["has_options"];
  String theOptions = "";
    int loop = data["loop"];
  if(has_options>0){
  #if DEBUG == 1
    USE_SERIAL.print("loop");
    USE_SERIAL.println(loop);
    USE_SERIAL.println("it has more options");
  #endif
    for(int lup=0; lup<loop; lup++){
    String valU=data["options"][lup];
    theOptions += valU;
    theOptions += " ";
  }
  #if DEBUG == 1
    USE_SERIAL.println(theOptions);
  #endif
  }
  else{
    loop = 0;
  #if DEBUG == 1
    USE_SERIAL.println("it has no options");
  #endif
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
  checkbalstr += history_id;
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
    while(Serial1.availableForWrite() < 1) {} 
    Serial1.print(checkbalstr); 
    Serial1.flush();
  }
  else if (slot_id > 5 && slot_id < 11) {
  checkbalstr += (slot_id - 5);
    while(Serial.availableForWrite() < 1) {}
   Serial.print(checkbalstr);
    Serial.flush(); 
  }
  else if (slot_id > 10 && slot_id < 16) {
  checkbalstr += (slot_id - 10);
    while(Serial2.availableForWrite() < 1) {}
    Serial2.print(checkbalstr);
    Serial2.flush(); 
  }
}

void handleSockEvent(WStype_t type, uint8_t * payload, size_t length) {
	switch(type) {
		case WStype_DISCONNECTED:
    #if DEBUG == 1
			USE_SERIAL.printf("[WSc] Disconnected!\n");
    #endif
			break;
		case WStype_CONNECTED: {
    #if DEBUG == 1
			USE_SERIAL.printf("[WSc] Connected to url: %s\n", payload);
    #endif
			// send message to server when Connected
			ptrWebSocket->sendTXT("Connected");
		}
			break;
		case WStype_TEXT:
    #if DEBUG == 1
			USE_SERIAL.printf("[WSc] get text: %s\n", payload);
    #endif
			// send message to server
			// ptrWebSocket->sendTXT("message here");
			break;
		case WStype_BIN:
    #if DEBUG == 1
			USE_SERIAL.printf("[WSc] get binary length: %u\n", length);
    #endif
      //hexdump(payload, length);

      // send data to server
      // ptrWebSocket->sendBIN(payload, length);
    break;
    case WStype_PING:
        // pong will be send automatically
      #if DEBUG == 1
        USE_SERIAL.printf("[WSc] get ping\n");
      #endif
        break;
    case WStype_PONG:
        // answer to a ping we send
      #if DEBUG == 1
        USE_SERIAL.printf("[WSc] get pong\n");
      #endif
      break;
    default:
      break;
    }

}
