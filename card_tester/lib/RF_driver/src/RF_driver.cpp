#include "RF_driver.h"

loRa_spi LoRa_spi;

bool loRa_spi::begin(){
    pinMode(CS2, OUTPUT);
    pinMode(CS1, OUTPUT);
    digitalWrite(CS2, HIGH);
    digitalWrite(CS1, HIGH);
    spi.begin(RF_SCK, RF_MISO, RF_MOSI);
    

    //LBAA.setTCXO(1.6, 5000);
    LBAA.XTAL = true;
    int16_t result_Sband = LBAA.begin(864.0,125.0,9,7,18,10,8,1.6);
    
    //int16_t result_UHF = RFM.begin(434.0,125.0,9,7,18,10,8,0); //Kører AGC og 434 MHz

    
    if(result_Sband == RADIOLIB_ERR_NONE){
        Serial.println("module says: HIII");
        return 1;
    }
    else{
        Serial.printf("RADIO says %d", result_Sband);

        return 0;
    }


}