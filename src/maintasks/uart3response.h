/*************************************************************************************************
 * Uart3Response.h
 * 
 * Inherits from the Abstract Task Class
 *
 * Purpose: Export functionality to separate .h files from main code
 *
 * **********************************************************************************************/
#ifdef ARDUINO_ESP32_DEV
#ifndef UART3RESPONSE_H_
#define UART3RESPONSE_H_

// ***
// *** The Uart3Response class helps to recieve commands from the
// *** main CPU and sends it to the appropraite GSM Module.
// ***

#include <config.h>

/*****************************************************************************************
* Class:    Uart3Response
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

//Define the Uart3Response class as type Task
class Uart3Response : public Task
{
    private:
        SocketIoClient *socketio_ptr;
        HardwareSerial mySerial;
    public:
        Uart3Response(SocketIoClient *);
        void debugWrite(String debugMsg);       //Used for printing dubug messages from other tasks
        void processInput(String data);
        virtual void run(uint32_t now);         //
        virtual bool canRun(uint32_t now);      //
};

//Uart3Response Constructor with ReadSms Object as param
Uart3Response::Uart3Response(SocketIoClient *_socketio_ptr)
: Task(),
socketio_ptr(_socketio_ptr),
mySerial(3)
{
    mySerial.begin(HARD_BAUD, SERIAL_8N1, RXD3, TXD3);
}

bool Uart3Response::canRun(uint32_t now)
{
    return mySerial.available() > 0;
}


void Uart3Response::run(uint32_t now)
{
    processInput(mySerial.readStringUntil(TERMINATION_CHAR));
}

void Uart3Response::processInput(String data)
{
    String datasub = data.substring(0, CMD_LEN);
    const char *response = data.substring(CMD_LEN + 2, data.length()-1).c_str();

    if (datasub == READSMS) {
        socketio_ptr->emit(READSMS_ADDR, response);
    }
    else if (datasub == DAILUSSD) {
        socketio_ptr->emit(DAILUSSD_ADDR, response);
    }
    else if (datasub == GSMSTATUS) {
        socketio_ptr->emit(GSMSTATUS_ADDR, response);
    }
    else if (datasub == RESTARTGSM) {
        socketio_ptr->emit(RESTARTGSM_ADDR, response);
    }
    else if (datasub == FEEDBACK) {
        //TODO: process response and send back to the GSM Module
    }
}

// ***
// *** Uart3Response::debugWrite() <--provides basic debug info from other tasks
// ***
void Uart3Response::debugWrite(String debugMsg)
{
	Serial.println(debugMsg);
}

#endif
#endif