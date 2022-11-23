
/****   Include Libraries    ****/
#include <Arduino.h>

#include "sdkconfig.h"
// #include "esp_core_dump.h"

//init crash handler
extern "C"
{
#include "esp_core_dump.h"
}
void readCoreDump();
esp_err_t esp_core_dump_image_erase();
//crash init end



#include <Preferences.h>
Preferences preferences;

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

String broker_url ;  
String mqtt_username; 
String mqtt_password;
uint16_t broker_port; 
String secret_token;



#if deploy == 0
  #include "defaultPreference.h" 
#endif

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
// SocketIoClient websocket;
WebSocketsClient websck;


void blinkerLED( void * pvParameters );
void CheckSims( void * pvParameters );


// void wsEmit(const char *payload);

TaskHandle_t SocketioHandle;
TaskHandle_t sim_check;

// #define LOG_LOCAL_LEVEL ESP_LOG_INFO
// #include <esp_rom_gpio.h>

void setup() {

  
  // esp_log_level_set("*", ESP_LOG_INFO); // disable ONLY 2nd stage output
  // esp_log_level_set("*", ESP_LOG_ERROR); // disable ONLY 2nd stage output
  // esp_log_level_set("*", ESP_LOG_WARN); // disable ONLY 2nd stage output
  setuplcd();
  lcdloop();
  pinMode(SLOT_RST1, OUTPUT);
  pinMode(SLOT_RST2, OUTPUT);
  pinMode(SLOT_RST3, OUTPUT);
  // turn off all sim module
  digitalWrite(SLOT_RST1,LOW);
  digitalWrite(SLOT_RST2,LOW);
  digitalWrite(SLOT_RST3,LOW);
  
  delay(1000); 
  display.clear();
  display.setFont(ArialMT_Plain_10);
  display.drawString(0, 10, "Initialising all parameters");
  display.display();
  
  pinMode(LED_BLINKER, OUTPUT);
  pinMode(34, OUTPUT);

  

  Serial.begin(HARD_BAUD);
  
  Serial1.begin(HARD_BAUD, SERIAL_8N1, RXD1, TXD1);
  
  Serial2.begin(HARD_BAUD, SERIAL_8N1, RXD2, TXD2);


  #if(DEBUG == 1)
  String plainText = "Tech tutorials xTech tutorials xxyzgvszufsdgftzsdfgsdfzfsfdzfsdzfsdtzfdtzsfdtzsfdtzfsdtzfstzfzsfdfzs";

  String encodedString = base64::encode(plainText);
  Serial.print("\nSetting cipher key: ");
  Serial.println(key);
  cipher->setKey(key);

  Serial.println("\nOriginal plain text:");
  Serial.println(encodedString);

  Serial.println("\nCiphered text:");
  String text = cipher->encryptString(encodedString);
  Serial.println(text);

  Serial.println("\nDecoded text:");
  Serial.println(cipher->decryptString(text));



  Serial.print("using Version ");
  Serial.println(ESP32firmware_version);
  Serial1.println("Serial 1");
  Serial2.println("Serial 2");
  #endif
    

preferences.begin("my-app", false); 
ESP32firmware_version = preferences.getFloat("version", 1);
#if deploy == 0
  setupPreference();
#endif

broker_url = preferences.getString("brokeURL","  ");
mqtt_username  = preferences.getString("mqttUSN", "  ");
mqtt_password  = preferences.getString("mqttPSD", "  ");
broker_port  = preferences.getInt("mqttPORT", 1000);
secret_token  = preferences.getString("mqttSekTok", "   ");
device_name = preferences.getString("device_name", "  ");
serverName = preferences.getString("serverName", "  ");

// #if deploy == 0
  Serial.print("serverName is ");
  Serial.println(serverName);
  Serial.print("device_name is ");
  Serial.println(device_name);
  Serial.print("secret_token is ");
  Serial.println(secret_token);
  Serial.print("broker_port is ");
  Serial.println(broker_port);
  Serial.print("mqtt_password is ");
  Serial.println(mqtt_password);
  Serial.print("mqtt_username is ");
  Serial.println(mqtt_username);
  Serial.print("broker_url is ");
  Serial.println(broker_url);
  Serial.print("ESP32firmware_version is ");
  Serial.println(ESP32firmware_version);
// #endif


preferences.end();


// to save a messege use createAndWriteFile(String buff); where buff is the message from the promini/sim module
  
  xTaskCreatePinnedToCore(  //PIN socketIO to core 1
    blinkerLED,         //Point to the tasks function
    "blinkerTask",       //name indentifier for the task
    20480,                    //stack size of the task
    NULL,                     //parameters of the task
    2,                        //priority of the task
    &SocketioHandle,         //  Hanlder variable for the task
    1
    );

      
    #if (hasMMC == 1)
      initCard();
      
      readCoreDump();

      #if(TEST_CASH_DUMP==1)
          for (int i = 0; i < 10; i++)
            {
              digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
              Serial.println("Hello!");
              delay(500);
            }
            Serial.println("RESTART ::");
            int a = 0;
            int b = 4;
            Serial.printf("%d\n", b / a);
      #endif
    #endif


delay(2000);
  display.setFont(ArialMT_Plain_10);
  display.drawString(0, 20, "Connecting to Wifi..");
  display.display();
  WiFi.disconnect(true);
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
    
  display.setFont(ArialMT_Plain_10);
  display.drawString(0, 20, "Connecting to Wifi..: OK");
  
  display.setFont(ArialMT_Plain_10);
  display.drawString(0, 30, "Connecting to Mqtt..");
  display.display();

#if(channel == MqttChannel)
  setupDateTime();
  mqtt_client.setBufferSize(1024);
  setMqttClientPtr(&mqtt_client);
  setWebsocketPtr(&client);
  initMqtt();
  display.setFont(ArialMT_Plain_10);
  display.drawString(0, 30, "Connecting to Mqtt..: OK");
  display.display();
	// server address, port and URL
  
  display.setFont(ArialMT_Plain_10);
  display.drawString(0, 40, "Resolving DNS..");
  display.display();
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
  display.setFont(ArialMT_Plain_10);
  display.drawString(0, 40, "Resolving DNS..: OK");
  display.display();
  }
  else{
    
  display.setFont(ArialMT_Plain_10);
  display.drawString(0, 40, "Resolving DNS..: FAIL");
  display.display();
  }
#endif
delay(2000);
      SimSlot1.flush();
      SimSlot2.flush();
      SimSlot3.flush();
    xTaskCreatePinnedToCore(  //PIN socketIO to core 1
    CheckSims,         //Point to the tasks function
    "simcardCheck",       //name indentifier for the task
    20480,                    //stack size of the task
    NULL,                     //parameters of the task
    2,                        //priority of the task
    &sim_check,         //  Hanlder variable for the task
    1
    );      
     
    
      // SimSlot1.println("ATD+2348161111269;"); //uncomment to test sim modules
      // SimSlot2.println("ATD+2348161111269;"); //uncomment to test sim modules
      // SimSlot3.println("ATD+2348161111269;"); //uncomment to test sim modules




	xTaskCreate(check_update_task, "check_update_task", 8192, NULL, 5, NULL);

}

void loop() {

   displayInfo();
   loopSocket();
   keepComms();
   
  if(reloadFlag==1){  // if a new update from OTA is avalable
    Serial.println("restarting system now!!!");
    vTaskDelete( sim_check );
    digitalWrite(SLOT_RST1,LOW);
    digitalWrite(SLOT_RST2,LOW);
    digitalWrite(SLOT_RST3,LOW);
    delay(5000);
    esp_restart();
  }
  else{
    if(mqtt == 1 ){
      if(SimSlot3.available() >= 1){  //slot 3 working
      #if (DEBUG == 1)
        USE_SERIAL.print(F("REACH HERE: slot 3"));
      #endif
        SimSlot3getResponse();
      }
      if(SimSlot2.available() >= 1){
      #if (DEBUG == 1)
        USE_SERIAL.print(F("REACH HERE: slot 2"));
      #endif
        SimSlot2getResponse();
      }
      if(SimSlot1.available() >= 1){
      #if (DEBUG == 1)
        USE_SERIAL.print(F("REACH HERE: slot 1"));
        Serial.println(" pingVerified is = 0");
      #endif
        SimSlot1getResponse();
      }

    if(slot1Taskflag ==1 ){
      #if(DEBUG == 1) 
      USE_SERIAL.println("am here 1");
      #endif
      slot1Task();
    }
    if(slot2Taskflag ==1 ){
      #if(DEBUG == 1) 
      USE_SERIAL.println("am here 2");
      #endif
      slot2Task();
    }
    if(slot3Taskflag ==1 ){
      #if(DEBUG == 1) 
      USE_SERIAL.println("am here 3");
      #endif
      slot3Task();
    }

    }
    else{// mqtt is inactive, i should deactivate sim module
      

    }

  }
                


}

void CheckSims( void * pvParameters ) // this should keep running in background
{
 for( ;; )
 {
   //first hard restart any module with no sim
   if(sim1_active == 0){
    slot1Taskflag = 0;
     digitalWrite(SLOT_RST1,LOW);
   }
   if(sim2_active == 0){
    slot2Taskflag = 0;
     digitalWrite(SLOT_RST2,LOW);
   }
   if(sim3_active == 0){
    slot3Taskflag = 0;
     digitalWrite(SLOT_RST3,LOW);
   }
   vTaskDelay(120);
   digitalWrite(SLOT_RST1,HIGH);
   digitalWrite(SLOT_RST3,HIGH);
   digitalWrite(SLOT_RST2,HIGH);
   
   vTaskDelay(3000);

  // second reconfigure and check for sim
   if(sim1_active == 0){
        SimSlot1.println("ATE0");
     vTaskDelay(200);
        SimSlot1.println("AT+CMGF=1");
     vTaskDelay(200);
     SimSlot1.println("AT+CPIN?");
   }
   if(sim2_active == 0){
        SimSlot2.println("AT+CMGF=1");
     vTaskDelay(200);
        SimSlot2.println("ATE0");
     vTaskDelay(200);
     SimSlot2.println("AT+CPIN?");//0695368041
   }
   if(sim3_active == 0){
        SimSlot3.println("AT+CMGF=1");
     vTaskDelay(200);
        SimSlot3.println("ATE0");
     vTaskDelay(200);
     SimSlot3.println("AT+CPIN?");
   }
     vTaskDelay(800);

   // third is to get sim get the service provider name from the SIM and specify how newly arrived SMS messages should be handled
   if(sim1_active == 1){
    //  SimSlot1.println("AT+CSPN?"); // this provice Service provider name, i dont like this
     SimSlot1.println("AT+COPS?");    // i prefer operator name 
    //  SimSlot1.println("AT+CNMI?");
   }
   if(sim2_active == 1){
        // SimSlot2.println("AT+CSPN?");
        SimSlot2.println("AT+COPS?"); 
        // SimSlot2.println("AT+CNMI?");
   }
   if(sim3_active == 1){
        // SimSlot3.println("AT+CSPN?");
        SimSlot3.println("AT+COPS?"); 
        // SimSlot3.println("AT+CNMI?");
   }
     vTaskDelay(2000);

   // lastly, read all stored sms's (if any) and push to sever
    if(sim1_active == 2){
      sim1_active = 3;
      slot1Taskflag = 1;
      // slot1Task(); // check for sms that on sim and read them
      }
    if(sim2_active == 2){
      slot2Taskflag = 1;
      sim2_active = 3;
      // slot2Task(); // check for sms that on sim and read them
      }
    if(sim3_active == 2){
      sim3_active = 3;
      slot3Taskflag = 1;
      // slot3Task(); // check for sms that on sim and read them
      }
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
        Serial.println("Blink ON");
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
  /* TODO:::
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



///core dump function


void readCoreDump()
{
  size_t size = 0;
  size_t address = 0;
  if (esp_core_dump_image_get(&address, &size) == ESP_OK)
  {
    const esp_partition_t *pt = NULL;
    pt = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_COREDUMP, "coredump");

    if (pt != NULL)
    {
      uint8_t bf[256];
      char str_dst[640];
      int16_t toRead;

      for (int16_t i = 0; i < (size / 256) + 1; i++)
      {
        strcpy(str_dst, "");
        toRead = (size - i * 256) > 256 ? 256 : (size - i * 256);

        esp_err_t er = esp_partition_read(pt, i * 256, bf, toRead);
        if (er != ESP_OK)
        {
          Serial.printf("FAIL [%x]\n",er);
          //ESP_LOGE("ESP32", "FAIL [%x]", er);
          break;
        }

        for (int16_t j = 0; j < 256; j++)
        {
          char str_tmp[3];

          sprintf(str_tmp, "%02x", bf[j]);
          strcat(str_dst, str_tmp);
        }

        printf("%s", str_dst);
      }
    }
    else
    {
      Serial.println("Partition NULL");
      //ESP_LOGE("ESP32", "Partition NULL");
    }
    esp_core_dump_image_erase();
  }
  else
  {
    Serial.println("esp_core_dump_image_get() FAIL");
    //ESP_LOGI("ESP32", "esp_core_dump_image_get() FAIL");
  }
}

esp_err_t esp_core_dump_image_erase()
{
    /* Find the partition that could potentially contain a (previous) core dump. */
    const esp_partition_t *core_part = esp_partition_find_first(ESP_PARTITION_TYPE_DATA,
                                                                ESP_PARTITION_SUBTYPE_DATA_COREDUMP,
                                                                "coredump");
    if (!core_part) {
        Serial.println("No core dump partition found!");
        return ESP_ERR_NOT_FOUND;
    }
    if (core_part->size < sizeof(uint32_t)) {
        Serial.println("Too small core dump partition!");
        return ESP_ERR_INVALID_SIZE;
    }

    esp_err_t err = ESP_OK;
    err = esp_partition_erase_range(core_part, 0, core_part->size);
    if (err != ESP_OK) {
        Serial.printf("Failed to erase core dump partition (%d)!\n", err);
        return err;
    }

    // on encrypted flash esp_partition_erase_range will leave encrypted
    // garbage instead of 0xFFFFFFFF so overwriting again to safely signalize
    // deleted coredumps
    const uint32_t invalid_size = 0xFFFFFFFF;
    err = esp_partition_write(core_part, 0, &invalid_size, sizeof(invalid_size));
    if (err != ESP_OK) {
        Serial.printf("Failed to write core dump partition size (%d)!\n", err);
    }

    return err;
}
