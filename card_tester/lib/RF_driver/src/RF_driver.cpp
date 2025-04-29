#include "RF_driver.h"

loRa_spi LoRa_spi;

bool loRa_spi::begin(){
    pinMode(CS2, OUTPUT); 
    spi.begin(RF_SCK, RF_MISO, RF_MOSI);
    
    LBAA.XTAL = true;

    int16_t result = LBAA.begin(434.0,125.0,9,7,18,10,8,0);
    
    Serial.printf("module says: %d", result);
    if(result == RADIOLIB_ERR_NONE){
        return 1;
    }
    else{
        return 0;
    }
}