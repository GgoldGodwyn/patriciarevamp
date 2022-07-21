#include "Arduino.h"


//read a directory
 void readRecord(String failedData){
     String dataa="";
       myFile = SD.open(failedData);
  if (myFile) {
    Serial.print(failedData);
    Serial.print(" is ");
 
    // read from the file until there's nothing else in it:
    while (myFile.available()) {
      dataa.concat((char)myFile.read());
    }
    // close the file:
    myFile.close();
  } else {
    // if the file didn't open, print an error:
    Serial.print("error opening ");
    Serial.println(failedData);
  }
  // Serial.print(" file has data : ");
  // Serial.println(dataa);
  // to avoid using String to hold a large string, load all into a log in the memory card and have all buffered out to the server at once
    // loga.println(dataa); 
    // replySocket(dataa); 
 /*   String datasub = base64::encode(dataa);
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

      dataa = "";

      serializeJson(doc, dataa);
      //wsEmit(data.c_str());
      websocket.emit(READSMS_ADDR, dataa.c_str());*/
    
 }

