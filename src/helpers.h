#ifndef HELPERS_H_
#define HELPERS_H_

#include <arduino.h>
#include "config.h"

class Helpers
{
private:
#ifdef ARDUINO_ESP32_DEV
        HardwareSerial my_serial;
#else
        SoftwareSerial my_serial;
#endif

public:
#ifdef ARDUINO_ESP32_DEV
    Helpers(HardwareSerial );
#else
    Helpers(SoftwareSerial );
#endif
    boolean checkForExptReplyinRcvdString(String recvdString, const char* exptReply);
    String  readReply(uint32_t timeout);
    String readUssdReply(uint32_t timeout);
    String  readReply( uint32_t timeout, uint8_t expctd_num_of_lines);
    void  readReply( char buffer[],uint32_t timeout, uint8_t expctd_num_of_lines);
    boolean waitResponse(const char* exptReply, uint32_t timeout);
    boolean waitResponse(const char* exptReply, uint32_t timeout, uint8_t noOflines);
    void serialFlush();
#ifdef ARDUINO_ESP32_DEV
    void setSerialStream(HardwareSerial );
#else
    void setSerialStream(SoftwareSerial );
#endif
};
#ifdef ARDUINO_ESP32_DEV
Helpers::Helpers(HardwareSerial _my_serial) : my_serial(_my_serial)
{
}

// Set the serial port to be used for stream
void Helpers::setSerialStream(HardwareSerial stream)
{
    my_serial = stream;
}

#else
Helpers::Helpers(SoftwareSerial _my_serial) : my_serial(_my_serial)
{
}

// Set the serial port to be used for stream
void Helpers::setSerialStream(SoftwareSerial stream)
{
    my_serial = stream;
}
#endif

// Read reply from gsm module
String Helpers::readReply(uint32_t timeout)
{
  uint32_t previousTime = millis();
  String receivedString = "";
  #ifdef ARDUINO_AVR_PRO
    my_serial.listen();
  #endif
  while (millis() - previousTime < timeout)
  {
    if (my_serial.available())    //AT/R/N   /R/N/OK/R/N
    {
      char inChar = (char(my_serial.read()));
      if ((inChar != '\n') && (inChar != '\r'))
      { /*Append data till enter character received*/
        receivedString += inChar;
        // previousTime = millis();
      }
      else
      {
        return receivedString;
      }
    }
  }
  return receivedString;
}

// Read reply from gsm module
String Helpers::readUssdReply(uint32_t timeout)
{
  uint32_t previousTime = millis();
  String receivedString = "";
  #ifdef ARDUINO_AVR_PRO
    my_serial.listen();
  #endif
  while (millis() - previousTime < timeout)
  {
    if (my_serial.available())    //AT/R/N   /R/N/OK/R/N
    {
      char inChar = (char(my_serial.read()));
      if ((inChar != '\n') && (inChar != '\r'))
      { /*Append data till enter character received*/
        receivedString += inChar;
        // previousTime = millis();
      }
      else
      {
      }
    }
  }
  return receivedString;
}


// Read certain lines reply from gsm module
String Helpers::readReply(uint32_t timeout, uint8_t expctd_num_of_lines)
{
  uint8_t line_number = 0;
  uint32_t previousTime = millis();
  String receivedString = "";
  #ifdef ARDUINO_AVR_PRO
    my_serial.listen();
  #endif
  while (millis() - previousTime < timeout)
  {
    if (my_serial.available())
    {
      char inChar = (char)my_serial.read();
      if ((inChar != '\n') && (inChar != '\r'))
      { /*Append data till enter character received*/
        receivedString += inChar;
        // previousTime = millis();
      }
      else if ((inChar == '\n'))
      {
        if (line_number < expctd_num_of_lines)
        {
          line_number++;
        }
        else
        {
          break;
        }
        receivedString += inChar;
      }
      else if (inChar == '\r')
      {
        receivedString += inChar;
        if (line_number >= expctd_num_of_lines)
        {
          break;
        }
      }

    }

  }
  // Serial.print("i bad boi : ");
  // Serial.println(receivedString);

  return receivedString;

}


// Read certain lines reply from gsm module
void Helpers::readReply(char buffer[],uint32_t timeout, uint8_t expctd_num_of_lines)
{
  uint8_t line_number = 0;
  uint32_t previousTime = millis();
  memset(buffer,'\0', strlen(buffer));
  int ne = 0;
  #ifdef ARDUINO_AVR_PRO
    my_serial.listen();
  #endif

  char inChar;
  while (millis() - previousTime < timeout )
  {
    if(my_serial.available() && ne <99 ){
      inChar = (char)my_serial.read();
      // buffer += inChar;
      // sprintf(buffer,"%s%c",buffer,inChar);
      buffer[ne]=inChar;
      ne++;
      if(inChar=='\n'){
        line_number++;
          // Serial.print("\nnumber of lines : ");
          // Serial.println(line_number);
          // Serial.print(" : ");
          // Serial.println(buffer);
      }
      if(line_number>=expctd_num_of_lines){
        break;
      }
        
    }
  }
  // Serial.print(" from helper : ");
  // Serial.println(buffer);

  // String rc = buffer;
  // return rc;

}


// Ensure response is read from gsm module
boolean Helpers::waitResponse(const char* exptReply, uint32_t timeout) {
  String inputString = "";
  inputString = readReply(timeout);
  // Serial.print("i got : ");
  // Serial.println(inputString);

#if (DEBUG == 1)
  Serial.println(inputString);
#endif
  boolean response = checkForExptReplyinRcvdString(inputString, exptReply);
  return response;
}
// Ensure response is read from gsm module
boolean Helpers::waitResponse(const char* exptReply, uint32_t timeout, uint8_t noOflines) {
  String inputString = "";
  inputString = readReply(timeout, noOflines);
#if (DEBUG == 1)
  Serial.println(inputString);
#endif
  boolean response = checkForExptReplyinRcvdString(inputString, exptReply);
  return response;
}
// Ensure the response contains the expected reply
boolean Helpers::checkForExptReplyinRcvdString(String recvdString, const char* exptReply)
{
  // Serial.print("exptReply : ");
  // Serial.print(exptReply);
  // Serial.print(" ::  recvdString : ");
  // Serial.println(recvdString);
  if (strstr(recvdString.c_str(), exptReply) != 0)
  {
        // Serial.println("Got Correct Reply");
    #if (DEBUG == 1)
        Serial.println("Got Correct Reply");
    #endif
        recvdString = "";
        return true;
    }
    else
    {
    #if (DEBUG == 1)
        Serial.println("Got ERROR");
    #endif
    recvdString = "";
    return false;
  }

}
// CLeanup the serial terminal
void Helpers::serialFlush() {
  while (my_serial.available() > 0) {
    my_serial.read();
  }
}
#endif