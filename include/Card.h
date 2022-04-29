#include <SPI.h>
#include <SD.h>
#include <Update.h>
#include <FS.h>


#define SD_CS      27  //32 //15
#define SD_SCK     26 //18 //2 //14 // PWN_EN pin
#define SD_MOSI    25 //23 //13
#define SD_MISO    33 //19 //12

File myFile,loga;
SPIClass SDSPI(HSPI);


void updateFromFS(fs::FS &fs);



void printDirectory(File dir, int numTabs) {
  while (true) {
    File entry =  dir.openNextFile();
    if (! entry) {
      // no more files
      break;
    }
    for (uint8_t i = 0; i < numTabs; i++) {
      Serial.print('\t');
    }
    #if (DEBUG == 1)
    Serial.print(entry.name());
    #endif
    if(entry.name() == loga.name()){
      // do nothing
    }
    else{  // read it and add it to log
      // readRecord(entry.name());
      Serial.println(entry.name());
    }
    
    if (entry.isDirectory()) {
      Serial.println("/");
      printDirectory(entry, numTabs + 1);
    } else {
      // files have sizes, directories do not
      Serial.print("\t\t");
      Serial.println(entry.size(), DEC);
    }
    entry.close();
  }
}

void initCard(){
    
  
    SDSPI.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS);
    SD.begin(SD_CS, SDSPI, 15000000); //16000000 as maximum speed
    
    uint8_t cardType = SD.cardType();

    if(cardType == CARD_NONE){
        Serial.println("No SD card attached"); // used for debug/// will not affect the program
        return;
    }else{
      updateFromFS(SD);
      
  }

 #if (DEBUG == 1)   
    Serial.print("Card type: ");
if(cardType == CARD_MMC){
Serial.println("MMC");
} else if(cardType == CARD_SD){
Serial.println("SDSC");
} else if(cardType == CARD_SDHC){
Serial.println("SDHC");
} else {
Serial.println("UNKNOWN");
}

Serial.print("Card size (Mb): ");
Serial.println((int)(SD.cardSize()/1048576));

Serial.print(" Available (Mb):");
Serial.println((int)(SD.totalBytes()/1048576));

 Serial.print("SD.cardSize() : ");
 Serial.println(SD.cardSize());
Serial.print(" Used (Mb):");
Serial.println((int)(SD.usedBytes()/1048576));
#endif

}

void ReadAllDirectory(){
  // SD.remove("/full_log.txt");
  loga = SD.open("/full_log.txt",FILE_WRITE);

  myFile = SD.open("/");
  printDirectory(myFile, 0);
  myFile.close();
  loga.close();
}

/*void stripeData(String buff,String * d_address,String * d_name,String * d_myMessage,String * d_time,String * d_phone, String *d_date){
   /  * // template \\
  aaaa
+CMGR: "REC UNREAD","a2c","","21/08/12,14:37:45+04"
        name          address        time                          

Trans User 1 you have been credited with N1814 by the code 08080808080
  message                                                         phone
OK
*  /

  // aaaa +CMGR: "REC UNREAD","a2c","","21/08/12,14:37:45+04" Trans User 1 you have been credited with N1814 by the code 08080808080 OK

// var message = base64Decode(e.data)
        buff.replace("aaaa", "");
        buff.replace("\n", "");
        buff.replace("\r", "");
        buff.replace("OK", "");
        Serial.print("buff : ");
        Serial.println(buff);
        // items = message.split(',')
        buff = buff.substring(buff.indexOf("\",\"") + 3);
        *d_address =  buff.substring(0, buff.indexOf("\",\""));
        buff = buff.substring(buff.indexOf("\",\"") + 3);
        *d_name =  buff.substring(0, buff.indexOf("\",\""));
        buff = buff.substring(buff.indexOf("\",\"") + 3);
        *d_time =  buff.substring(0, buff.indexOf("\""));
        *d_date =  buff.substring(0, buff.indexOf(","));
        buff = buff.substring(buff.indexOf("+04\"") + 3);
        *d_myMessage =  buff.substring(1, buff.indexOf("OK"));

        / *
        TEST:
        setup(){
          String messageL = "aaaa +CMGR: \"REC UNREAD\",\"a2c\",\"\",\"21/08/12,14:37:45+04\" Trans User 1 you have been credited with N1814 by the code 08080808080 OK";
          String d_addressL;
          String d_nameL;
          String d_myMessageL;
          String d_timeL;
          String d_phoneL;
          String d_dateL;
          while(1){
            stripeData( messageL, &d_addressL, &d_nameL, &d_myMessageL, &d_timeL, &d_phoneL,&d_dateL);// striping data
            Serial.print("-------------------\nd_name : ");
            Serial.println(d_nameL);
            Serial.print("d_address : ");
            Serial.println(d_addressL);
            Serial.print("d_time : ");
            Serial.println(d_timeL);
            Serial.print("d_date");
            Serial.println(d_dateL);
            Serial.print("d_myMessage : ");
            Serial.println(d_myMessageL);
            Serial.print("-------------------"); 
            delay(3000);
          }
          }
              * /

}*/


void SDlogger(String Data){
  if(Data.indexOf("REC UNREAD")<0){
    return;
  }
  File sdcard_file = SD.open("/redundantSMSs.txt", FILE_APPEND); //FILE_WRITE //FILE_APPEND
  if (sdcard_file) {
    sdcard_file.print("Data : ");
    sdcard_file.print(Data);
    sdcard_file.println("");
    sdcard_file.close(); // close the file
  }
  #if(DEBUG == 1)
  Serial.println(".............................");
  #endif
}

void createAndWriteFile(String buff){
  String failedData;
  // #if (DEBUG == 1)
  buff ="aaaa\n+CMGR: \"REC UNREAD\",\"MTN\",\"\",\"21/08/12,14:37:45+04\"\nTrans User 1 you have been credited with N1814 by the code 08080808080\n\nOK";
  // #endif

  if(buff.indexOf("REC UNREAD")>0){
    failedData = buff.substring(buff.indexOf(',') + 1);
    failedData = failedData.substring(failedData.indexOf('\"') + 1);
    String telco = '/' + failedData.substring(0, failedData.indexOf('\"'));
    failedData = failedData.substring(failedData.indexOf(',') + 1);
    failedData = failedData.substring(failedData.indexOf(',') + 1);
    failedData = failedData.substring(failedData.indexOf('\"') + 1);
    failedData = failedData.substring(0, failedData.indexOf('\"'));
    failedData.replace(0x2F, '_');
    failedData.replace(',', ' ');
    failedData.replace(':', '_');
    telco +=' ';
  failedData = telco + "20"+failedData;
  failedData += ".txt";
  }


// Serial.println(failedData);
  myFile = SD.open(failedData, FILE_WRITE);
 
  // if the file opened okay, write to it:
  if (myFile) {
    myFile.println(buff);
    // close the file:
    myFile.close();
  } else {
    // if the file didn't open, print an error:
    Serial.print("error opening : ");
    Serial.println(failedData);
  }
}
  
  //clear redundant sms logs
void clearLog(){
  File sdcard_file = SD.open("/redundantSMSs.txt", FILE_WRITE); //FILE_WRITE //FILE_APPEND
  if (sdcard_file) {
    sdcard_file.println(" ");
    sdcard_file.close(); // close the file
  }
}

// card firmware update
void rebootEspWithReason(String reason){
    Serial.println(reason);
    delay(1000);
    ESP.restart();
}

// perform the actual update from a given stream
void performUpdate(Stream &updateSource, size_t updateSize) {
   if (Update.begin(updateSize)) {      
      size_t written = Update.writeStream(updateSource);
      if (written == updateSize) {
         Serial.println("Written : " + String(written) + " successfully");
      }
      else {
         Serial.println("Written only : " + String(written) + "/" + String(updateSize) + ". Retry?");
      }
      if (Update.end()) {
         Serial.println("OTA done!");
         if (Update.isFinished()) {
            rebootEspWithReason("Update successfully completed. Rebooting.");
         }
         else {
            Serial.println("Update not finished? Something went wrong!");
         }
      }
      else {
         Serial.println("Error Occurred. Error #: " + String(Update.getError()));
      }
   }
   else
   {
      Serial.println("Not enough space to begin OTA");
   }
}

// check given FS for valid update.bin and perform update if available
void updateFromFS(fs::FS &fs) {
   File updateBin = fs.open("/Riskyfirmware.bin");
   if (updateBin) {
      if(updateBin.isDirectory()){
         Serial.println("Error, Riskyfirmware is not a valid file");
         updateBin.close();
         return;
      }

      size_t updateSize = updateBin.size();

      if (updateSize > 0) {
         Serial.println("Try to start update");
         performUpdate(updateBin, updateSize);
      }
      else {
         Serial.println("Error, file is empty");
      }

      updateBin.close();
    
      // whe finished remove the binary from sd card to indicate end of the process
      fs.remove("/Riskyfirmware.bin");      
   }
   else {
      Serial.println("Could not load latest firmware from sd root");
   }
}
