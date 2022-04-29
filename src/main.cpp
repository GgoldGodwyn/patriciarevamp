
/****   Include Libraries    ****/
#include <Arduino.h>
#include "macros.h"


#include "mDNSResolver.h"

#define MqttChannel 0
#define SOC_IO 1
#define channel MqttChannel

#include <PubSubClient.h>


using namespace mDNSResolver;

// MQTT details
#define RECONNECT 10000UL
uint32_t last_reconnect_attempt = 0;
const char* broker_url = "cc1efd1a984746fa9c27447711a4c90e.s1.eu.hivemq.cloud";  
// const char* broker_url = "test.mosquitto.org";                    // Public IP address or domain name
const char* mqtt_username = "testiot";  // MQTT username
const char* mqtt_password = "rnrZjan6uBj83KS";  // MQTT password
uint16_t broker_port = 8883;   //8884;  // 1883;


//Declare Objects
WiFiClientSecure wifi_client;
// WiFiClient wifiClient;
PubSubClient mqtt_client(wifi_client);
// PubSubClient mqtt_client(wifiClient);
WebSocketsClient client;
WiFiUDP udp;
Resolver resolver(udp);

//

#if(channel == MqttChannel)
#include "maintasks/mqtthelper.h"
#else
#include "maintasks/sockethelper.h"
#endif


void blinkerLED( void * pvParameters );
//Declare Objects
SocketIoClient websocket;
WebSocketsClient websck;


void blinkerLED( void * pvParameters );
// void wsEmit(const char *payload);

TaskHandle_t SocketioHandle;



void setup() {
  
  pinMode(LED_BLINKER, OUTPUT);
  pinMode(34, OUTPUT);

  Serial.begin(HARD_BAUD);
  
  Serial1.begin(HARD_BAUD, SERIAL_8N1, RXD1, TXD1);
  
  Serial2.begin(HARD_BAUD, SERIAL_8N1, RXD2, TXD2);

  Serial.println("Serial 0");
  Serial1.println("Serial 1");
  Serial2.println("Serial 2");


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

      for(int t = 0; t<3; t++){
        SimSlot1.println("ATE0");
        SimSlot2.println("ATE0");
        SimSlot3.println("ATE0");
        delay(500);
        SimSlot1.println("AT+CMGF=1");
        SimSlot2.println("AT+CMGF=1");
        SimSlot3.println("AT+CMGF=1");
              
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


      SimSlot1.flush();
      SimSlot2.flush();
      SimSlot3.flush();
        // delay(500);
      // SimSlot1.println("ATD+2348161111269;");
      // SimSlot2.println("ATD+2348161111269;");
      // SimSlot3.println("ATD+2348161111269;");


      
    #if (hasMMC == 1)
      initCard();
        // slot1Task(); // check for sms that on sim and read them
        // slot2Task(); // check for sms that on sim and read them
        // slot3Task(); // check for sms that on sim and read them

      // createAndWriteFile(" aaaq");
      // readRecord("/redundantSMSs.txt");
    #endif

delay(2000);
initWiFi();
    #if(channel != MqttChannel)

      //Register Socketio events to handler functions and start it
      websocket.on("connect", connect);
      websocket.on("disconnected", disconnected);
      //  websocket.on("read_sms", readsms);
      websocket.on("checkbal", checkbal);
      websocket.on("message", message);
      //  websocket.on("read_sms_pend",read_sms_pend);
      websocket.on("execute_ussd",execute_ussd);
      websocket.on("logger",Logger);
      setSocketioPtr(&websocket);
      setWebsocketPtr(&websck);
      websocket.beginSSL(SERVER_HOST,SERVER_PORT,SERVER_URL);
    #endif
    

#if(channel == MqttChannel)
  setupDateTime();
  mqtt_client.setBufferSize(1024);
  setMqttClientPtr(&mqtt_client);
  setWebsocketPtr(&client);
  initMqtt();
	// server address, port and URL
  #if(DEBUG == 1)
  USE_SERIAL.print("Resolving ");
  USE_SERIAL.println(ws_domain);
  #endif

  resolver.setLocalIP(WiFi.localIP());

  IPAddress ip = resolver.search(ws_domain);
  for(int u = 0; u<5; u++){
    if (ip != INADDR_NONE && !ip.toString().equals("0.0.0.0")) {
      ws_domain_resolved = true;
      break;
    }
    #if(DEBUG == 1)
    USE_SERIAL.println("Not resolved");
    #endif
    ip = resolver.search(ws_domain);
  }
  #if(DEBUG == 1)
  USE_SERIAL.print("Resolved: ");
  USE_SERIAL.println(ip);
  #endif

  if (ws_domain_resolved) {
    client.begin(ip.toString(), WS_PORT);

    // use HTTP Basic Authorization this is optional remove if not needed
    //client.setAuthorization(ws_username, ws_userpass);

    // try ever 5000 again if connection has failed
    client.setReconnectInterval(WS_RECONNECT_INTERVAL);
    
    // start heartbeat (optional)
    client.enableHeartbeat(WS_PING_INTERVAL, WS_PONG_INTERVAL, WS_DISCONNECT_TIMEOUT_COUNT);

    // event handler
    client.onEvent(handleSockEvent);
  }
#endif

    slot1Task(); // check for sms that on sim and read them
    slot2Task(); // check for sms that on sim and read them
    slot3Task(); // check for sms that on sim and read them

}

/*
// and this
void reconnect() {
  // Loop until we're reconnected
  while (!mqtt_client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "device_1";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (mqtt_client.connect(clientId.c_str(),mqtt_username,mqtt_password)) {
      Serial.println("connected");
      //Once connected, publish an announcement...
      mqtt_client.publish("device_1/message", "hello world");
      // ... and resubscribe
      mqtt_client.subscribe(sub_read_sms);
    } else {
      Serial.print("failed,");
      Serial.print(mqtt_client.state());
      Serial.println("try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}



void setup2() {
  pinMode(LED_BLINKER, OUTPUT);
  pinMode(34, OUTPUT);

  Serial.begin(HARD_BAUD);
  
  Serial1.begin(HARD_BAUD, SERIAL_8N1, RXD1, TXD1);
  
  Serial2.begin(HARD_BAUD, SERIAL_8N1, RXD2, TXD2);

  Serial1.println("Serial1");
  Serial2.println("Serial2");
  
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

  delay(2000);
  initWiFi();
  setupDateTime();

  mqtt_client.setBufferSize(1024);
  setMqttClientPtr(&mqtt_client);   //modified
  setWebsocketPtr(&client);
  initMqtt();


//changed to 
  mqtt_client.setServer(broker_url, broker_port);
  mqtt_client.setCallback(mqttCallback);
  // reconnect();


	// server address, port and URL
  #if(DEBUG == 1)
  USE_SERIAL.print("Resolving ");
  USE_SERIAL.println(ws_domain);
  #endif

  resolver.setLocalIP(WiFi.localIP());

  IPAddress ip = resolver.search(ws_domain);
  for(int u = 0; u<5; u++){
    break; // remove this on deployment
  // while(true) {
    if (ip != INADDR_NONE && !ip.toString().equals("0.0.0.0")) {
      ws_domain_resolved = true;
      break;
    }
    #if(DEBUG == 1)
    USE_SERIAL.println("Not resolved");
    #endif
    ip = resolver.search(ws_domain);
  }
  #if(DEBUG == 1)
  USE_SERIAL.print("Resolved: ");
  USE_SERIAL.println(ip);
  #endif

  if (ws_domain_resolved) {
    client.begin(ip.toString(), WS_PORT);

    // use HTTP Basic Authorization this is optional remove if not needed
    //client.setAuthorization(ws_username, ws_userpass);

    // try ever 5000 again if connection has failed
    client.setReconnectInterval(WS_RECONNECT_INTERVAL);
    
    // start heartbeat (optional)
    client.enableHeartbeat(WS_PING_INTERVAL, WS_PONG_INTERVAL, WS_DISCONNECT_TIMEOUT_COUNT);

    // event handler
    client.onEvent(handleSockEvent);
  }

  // SerialBT.printf("Finish BOOTING...\n");
  Serial.flush();
  Serial1.flush();
  Serial2.flush();
}

void publishSerialData(char *serialData){
  if (!mqtt_client.connected()) {
    reconnect();
  }
  mqtt_client.publish(pub_message, serialData);
  mqtt_client.publish(sub_read_sms, "hi");
}

*/
//Program will run mainly here
void loop() {

  /*  
   client.loop();
   if (Serial.available() > 0) {
     char mun[501];
     memset(mun,0, 501);
     Serial.readBytesUntil( '\n',mun,500);
     publishSerialData(mun);
     Serial.println("sent");
   }
*/

  // GO! Run the scheduler - it never returns.
  // scheduler.runTasks();
   loopSocket();
   keepComms();
  
  if(SimSlot3.available() >= 1){  //slot 3 working
  #if (DEBUG == 1)
    USE_SERIAL.print(F("REACH HERE: Serial 2"));
  #endif
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
  // setupDateTime();
  del_DB=0;
}

del_DB++;
#endif





  vTaskDelay(300);
 }
}
