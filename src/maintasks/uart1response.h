/*************************************************************************************************
 * Uart1Response.h
 * 
 * Inherits from the Abstract Task Class
 *
 * Purpose: Export functionality to separate .h files from main code
 *
 * **********************************************************************************************/
#ifdef ARDUINO_ESP32_DEV
#ifndef UART1RESPONSE_H_
#define UART1RESPONSE_H_

// ***
// *** The Uart1Response class helps to recieve commands from the
// *** main CPU and sends it to the appropraite GSM Module.
// ***

#include <config.h>

/*****************************************************************************************
* Class:    Uart1Response
* Task Type:  Task 
* Purpose:  Other tasks use a pointer to the debugger object to output simple debug
*       messages while this example executes.
*       Tasks that use the debugger object are passed a reference to &debugger
*       in their respective constructors.
*
*       For example: Blinker(uint8_t _pin, uint32_t _rate, Debugger *_ptrDebugger);
*
*       To output debug information use: ptrDebugger->debugWrite("debug info");
*
******************************************************************************************/

//Define the Uart1Response class as type Task
class Uart1Response : public Task
{
    private:
        // ReadSms *read_sms_ptr;
        // DailUssd *dail_ussd_ptr;
        SocketIoClient *socketio_ptr;
        HardwareSerial mySerial;
    public:
        Uart1Response(SocketIoClient *);
        void debugWrite(String debugMsg);       //Used for printing dubug messages from other tasks
        void processInput(String data);
        virtual void run(uint32_t now);         //
        virtual bool canRun(uint32_t now);      //
};

//Uart1Response Constructor with ReadSms Object as param
Uart1Response::Uart1Response(SocketIoClient *_socketio_ptr)
: Task(),
socketio_ptr(_socketio_ptr)
{
    //mySerial.begin(HARD_BAUD, SERIAL_8N1, RXD1, TXD1);
}

bool Uart1Response::canRun(uint32_t now)
{
    //return mySerial.available() > 0;
}


void Uart1Response::run(uint32_t now)
{
    //processInput(mySerial.readStringUntil(TERMINATION_CHAR));
}

void Uart1Response::processInput(String data)
{
    String datasub = data.substring(0, CMD_LEN);
    String respString = data.substring(CMD_LEN + 2, data.length()-1);

    if (datasub == READSMS) {
        int inputStringLength = respString.length();
        char inputString[inputStringLength];
        respString.toCharArray(inputString, inputStringLength);
        int encodedLength = Base64.encodedLength(inputStringLength);
        char encodedString[encodedLength];
        Base64.encode(encodedString, inputString, inputStringLength);

        DynamicJsonDocument doc(encodedLength + 20);

        doc["message"] = encodedString;

        respString = "";

        serializeJson(doc, respString);
            socketio_ptr->emit(READSMS_ADDR, respString.c_str());
        }
    else if (datasub == DAILUSSD) {
        socketio_ptr->emit(DAILUSSD_ADDR, respString.c_str());
    }
    else if (datasub == GSMSTATUS) {
        socketio_ptr->emit(GSMSTATUS_ADDR, respString.c_str());
    }
    else if (datasub == RESTARTGSM) {
        socketio_ptr->emit(RESTARTGSM_ADDR, respString.c_str());
    }
    else if (datasub == FEEDBACK) {
        //TODO: process respString and send back to the GSM Module
    }
}

// ***
// *** Uart1Response::debugWrite() <--provides basic debug info from other tasks
// ***
void Uart1Response::debugWrite(String debugMsg)
{
	Serial.println(debugMsg);
}

#endif
#endif