//#include <Arduino.h>

#include <HTTPClient.h>
// #include <HTTPUpdate.h>
#include <ESP32httpUpdate.h>
float releaseVersion ;



/**
 * This is lets-encrypt-x3-cross-signed.pem
 */
const char* rootCACertificate = \
"-----BEGIN CERTIFICATE-----\n" \
"MIIEkjCCA3qgAwIBAgIQCgFBQgAAAVOFc2oLheynCDANBgkqhkiG9w0BAQsFADA/\n" \
"MSQwIgYDVQQKExtEaWdpdGFsIFNpZ25hdHVyZSBUcnVzdCBDby4xFzAVBgNVBAMT\n" \
"DkRTVCBSb290IENBIFgzMB4XDTE2MDMxNzE2NDA0NloXDTIxMDMxNzE2NDA0Nlow\n" \
"SjELMAkGA1UEBhMCVVMxFjAUBgNVBAoTDUxldCdzIEVuY3J5cHQxIzAhBgNVBAMT\n" \
"GkxldCdzIEVuY3J5cHQgQXV0aG9yaXR5IFgzMIIBIjANBgkqhkiG9w0BAQEFAAOC\n" \
"AQ8AMIIBCgKCAQEAnNMM8FrlLke3cl03g7NoYzDq1zUmGSXhvb418XCSL7e4S0EF\n" \
"q6meNQhY7LEqxGiHC6PjdeTm86dicbp5gWAf15Gan/PQeGdxyGkOlZHP/uaZ6WA8\n" \
"SMx+yk13EiSdRxta67nsHjcAHJyse6cF6s5K671B5TaYucv9bTyWaN8jKkKQDIZ0\n" \
"Z8h/pZq4UmEUEz9l6YKHy9v6Dlb2honzhT+Xhq+w3Brvaw2VFn3EK6BlspkENnWA\n" \
"a6xK8xuQSXgvopZPKiAlKQTGdMDQMc2PMTiVFrqoM7hD8bEfwzB/onkxEz0tNvjj\n" \
"/PIzark5McWvxI0NHWQWM6r6hCm21AvA2H3DkwIDAQABo4IBfTCCAXkwEgYDVR0T\n" \
"AQH/BAgwBgEB/wIBADAOBgNVHQ8BAf8EBAMCAYYwfwYIKwYBBQUHAQEEczBxMDIG\n" \
"CCsGAQUFBzABhiZodHRwOi8vaXNyZy50cnVzdGlkLm9jc3AuaWRlbnRydXN0LmNv\n" \
"bTA7BggrBgEFBQcwAoYvaHR0cDovL2FwcHMuaWRlbnRydXN0LmNvbS9yb290cy9k\n" \
"c3Ryb290Y2F4My5wN2MwHwYDVR0jBBgwFoAUxKexpHsscfrb4UuQdf/EFWCFiRAw\n" \
"VAYDVR0gBE0wSzAIBgZngQwBAgEwPwYLKwYBBAGC3xMBAQEwMDAuBggrBgEFBQcC\n" \
"ARYiaHR0cDovL2Nwcy5yb290LXgxLmxldHNlbmNyeXB0Lm9yZzA8BgNVHR8ENTAz\n" \
"MDGgL6AthitodHRwOi8vY3JsLmlkZW50cnVzdC5jb20vRFNUUk9PVENBWDNDUkwu\n" \
"Y3JsMB0GA1UdDgQWBBSoSmpjBH3duubRObemRWXv86jsoTANBgkqhkiG9w0BAQsF\n" \
"AAOCAQEA3TPXEfNjWDjdGBX7CVW+dla5cEilaUcne8IkCJLxWh9KEik3JHRRHGJo\n" \
"uM2VcGfl96S8TihRzZvoroed6ti6WqEBmtzw3Wodatg+VyOeph4EYpr/1wXKtx8/\n" \
"wApIvJSwtmVi4MFU5aMqrSDE6ea73Mj2tcMyo5jMd6jmeWUHK8so/joWUoHOUgwu\n" \
"X4Po1QYz+3dszkDqMp4fklxBwXRsW10KXzPMTZ+sOPAveyxindmjkW8lGy+QsRlG\n" \
"PfZ+G6Z6h7mjem0Y+iWlkYcV4PIWL1iwBi8saCbGS5jN2p8M+X+Q7UNKEkROb3N6\n" \
"KOqkqm57TH2H3eDJAkSnh6/DNFu0Qg==\n" \
"-----END CERTIFICATE-----\n";




#if(serverType == myHTTPS)
//Using HTTPS UPDATE
#include "HttpsOTAUpdate.h"

int UpdateInprogress = 0;

static HttpsOTAStatus_t otastatus;

void HttpEvent(HttpEvent_t *event)
{
    switch(event->event_id) {
        case HTTP_EVENT_ERROR:
            Serial.println("Http Event Error");
            break;
        case HTTP_EVENT_ON_CONNECTED:
            Serial.println("Http Event On Connected");
            break;
        case HTTP_EVENT_HEADER_SENT:
            Serial.println("Http Event Header Sent");
            break;
        case HTTP_EVENT_ON_HEADER:
            Serial.printf("Http Event On Header, key=%s, value=%s\n", event->header_key, event->header_value);
            break;
        case HTTP_EVENT_ON_DATA:
            break;
        case HTTP_EVENT_ON_FINISH:
            Serial.println("Http Event On Finish");
            break;
        case HTTP_EVENT_DISCONNECTED:
            Serial.println("Http Event Disconnected");
            break;
    }
}

void checkForESPupdate(){
    if((WiFi.status() == WL_CONNECTED)) {
      otalink = serverName + otalink;
      HttpsOTA.onHttpEvent(HttpEvent);
      Serial.println("Starting OTA");
      HttpsOTA.begin(otalink.c_str(), rootCACertificate); 

    Serial.println("Please Wait it takes some time ...");
    UpdateInprogress = 1;
    
    }
}

#endif



#if(serverType == myHTTP)
//Using HTTP UPDATE
  void checkForESPupdate() {
	// printf("HTTPS OTA, firmware %.1f\n\n", ESP32firmware_version);

  /*      HTTPClient http;
  String link = serverName + otalink;
  Serial.println(link);
      http.begin(link.c_str());
      int respCode = http.GET();
      Serial.println(respCode);
      Serial.println("done");*/
	
    if((WiFi.status() == WL_CONNECTED)) {
    // WiFiClientSecure client;
//     client.setCACert(rootCACertificate);
// // client, https://

		otalink = serverName + otalink;
    // Serial.println(serverName);
    // Serial.println(otalink);
      //http://192.168.43.31:8000/storage/otabins/otabin.bin
// Serial.println("awaiting response");
		ESPhttpUpdate.rebootOnUpdate(false);
// Serial.println("updating binary file");
        t_httpUpdate_return ret = ESPhttpUpdate.update(otalink.c_str());
// Serial.println("got response");
        switch(ret) {
            case HTTP_UPDATE_FAILED:
                USE_SERIAL.printf("HTTP_UPDATE_FAILD Error (%d): %s/r/n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
                break;

            case HTTP_UPDATE_NO_UPDATES:
                USE_SERIAL.println("HTTP_UPDATE_NO_UPDATES");
                break;

            case HTTP_UPDATE_OK:
                preferences.begin("my-app", false); 
                preferences.putFloat("version", releaseVersion );
                preferences.end();
                USE_SERIAL.println("HTTP_UPDATE_OK");
                reloadFlag=1;
                break;
        }
    }
	
}

#endif



//  compulsory task running in background
void check_update_task(void * pvParameters ) {
	while(1) {

        HTTPClient http;

      // String serverPath = serverName;
      String serverPath = serverName + "/api/ota-updates/latest";
      
      // Your Domain name with URL path or IP address with path
      #if(serverType == myHTTP)
      http.begin(serverPath.c_str());
      #else if(serverType == myHTTPS)
      http.begin(serverPath.c_str(), rootCACertificate);
      #endif

      
      http.addHeader("api-key","live-c67462b3-364c-4d57-bdf0-9f5a610965ee");
      // Send HTTP GET request
      int httpResponseCode = http.GET();
      
      if (httpResponseCode>0) {
        #if(DEBUG == 1)
        Serial.println(httpResponseCode);
        #endif
        String payload = http.getString();
        #if(DEBUG == 1)
          Serial.println(payload);
        #endif
        DynamicJsonDocument doc(1024);
        deserializeJson(doc, payload);

        releaseVersion = doc["data"]["version"];
        #if(DEBUG == 1)
          Serial.print("version is ");
          Serial.println(releaseVersion);
        #endif

        if(releaseVersion>ESP32firmware_version){
          #if(DEBUG == 1)
            printf("Looking for a new firmware...\n");
          #endif
          String buf = doc["data"]["file_path"];
          otalink = "/storage"+ buf ;  // /storage/gdht3.bin
            checkForESPupdate();
        }
        else{
          #if(DEBUG == 1)
            Serial.println("releaseVersion is lesser than or the same");
          #endif
        }
      }
      else {
        #if(DEBUG == 1)
          Serial.print("Error code: ");
          Serial.println(httpResponseCode);
        #endif
      }
      // Free resources
      http.end();

  #if(serverType == myHTTPS)
  if(UpdateInprogress == 1){
    otastatus = HttpsOTA.status();
    if(otastatus == HTTPS_OTA_SUCCESS) { 
      // todo, comment on DEBUG
        Serial.println("Firmware written successfully. To reboot device, call API ESP.restart() or PUSH restart button on device");
        esp_restart();
    } else if(otastatus == HTTPS_OTA_FAIL) {       
      // todo, comment on DEBUG
        Serial.println("Firmware Upgrade Fail");
    }
  }
  #endif

//1800000 30mins
        vTaskDelay(20000 / portTICK_PERIOD_MS);
    }
	}

