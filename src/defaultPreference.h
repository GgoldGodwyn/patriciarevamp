#include "Arduino.h"
#include <Preferences.h>
#include <nvs_flash.h>

void setupPreference(){

    #if wipeNVS
        nvs_flash_erase(); // erase the NVS partition and...
        nvs_flash_init(); // initialize the NVS partition.
    #endif


    #if (WritePef)
        preferences.clear();
        preferences.putString("brokeURL","cc1efd1a984746fa9c27447711a4c90e.s1.eu.hivemq.cloud");
        preferences.putString("mqttUSN","testiot");
        preferences.putString("mqttPSD","rnrZjan6uBj83KS");
        preferences.putInt("mqttPORT",8883);
        preferences.putString("mqttSekTok","VLbrvWIxL7KCpCJFK07Wxv67XikNZyvy6Hi31PXOO34=");
        preferences.putString("device_name",device_name);
        preferences.putString("serverName","http://atoc.gloverapp.co");
        preferences.end();
    #endif    
    
    Serial.println("rebuild and flash program with wipe_and_restore_memory = ReadPef !!!");
// while(1); // rebuild program with wipe_and_restore_memory = ReadPef
    // by default all read implementation happens inside the setup function for security reasons

}