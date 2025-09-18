#include "RadioLib.h"
#include "meas_struct.h"
//#define RADIOLIB_GODMODE (1) //Her gør vi lige noget sindsygt.
#define RADIOLIB_INTERRUPT_TIMING  (1) //bed til gud, og sørger for at vi kan få ting til at gå ok hurtigt igen.


#define CS1 47 //chip select for RFM
#define CS2 48 //chip select for LBAA
#define RF_SCK 18
#define RF_MOSI 17
#define RF_MISO 16


enum Freq_settings{NONE= 0, UHF =1, SBAND=2};

class loRa_spi{
    private:
    uint8_t bufferA[4] = {0}; //BUFFER TIL LBAA
    uint8_t bufferB[4] = {0}; //BUFFER TIL RFM96
    SPISettings spi = SPISettings(10000000, MSBFIRST, SPI_MODE0); // 10 MHz for safety 

    //Radiolib structs til LBAA & RFM
    LR1121 LBAA = new Module(CS2, RADIOLIB_NC, RADIOLIB_NC, RADIOLIB_NC, SPI, spi); 
    RFM96 RFM = new Module(CS1, RADIOLIB_NC, RADIOLIB_NC, RADIOLIB_NC, SPI, spi);
    


    volatile Freq_settings state = NONE;
    public:
    

    /// @brief Starts the chip at teh given settings
    /// @param bw %LoRa link bandwidth in kHz. Allowed values are 7.8, 10.4, 15.6, 20.8, 31.25, 41.7, 62.5, 125, 250 and 500 kHz 
    /// @param sf %LoRa link spreading factor. Allowed values range from 6 to 12.
    /// @return 
    bool begin(float bw = 250.0, uint8_t sf = 6);
    
    /// @brief Sets the mode for this driver, UHF will transmit on 433 MHz & 868 MHz, while SBAND transmits on 433 MHz & 2.4GHz
    /// @param Settings 
    void setup_settings_tx(Freq_settings Settings);

    /// @brief Transmits for both modules
    void transmit_all();

    /// @brief Poll'er begge radiochips og giver os true hvis vi har fået noget som helst.
    /// @param output Et struct med measurement RSSI samt frekvens, 
    /// @return 
    bool poll(Measurement *output);
    bool set_rx();
};

extern loRa_spi LoRa_spi;