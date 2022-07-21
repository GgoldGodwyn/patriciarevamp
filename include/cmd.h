#include "Arduino.h"

struct ATcommander{
    char buffer[1000];
  int Serial_port;
  ATcommander() : Serial_port(Serial_port) {}   //look, a constructor
    void SendAT(){
        Serial.println("hello struct2");
      }
        
    void clearall(void){
      if(Serial_port == 0){
        while (Serial.available()){
          Serial.read();
        }}
      if(Serial_port == 1){
        while (Serial1.available()){
          Serial1.read();
        }}
      if(Serial_port == 2){
        while (Serial2.available()){
          Serial2.read();
        }}
      }
       
};