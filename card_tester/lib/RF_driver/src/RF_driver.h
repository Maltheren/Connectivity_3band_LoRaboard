#include "RadioLib.h"
#define CS1 47 //chip select for RFM
#define CS2 48 //chip select for LBAA
#define RF_SCK 18
#define RF_MOSI 17
#define RF_MISO 16

class loRa_spi{
    private:
    SPIClass spi = SPIClass(FSPI);
    LR1121 LBAA = new Module(CS2, -1, RADIOLIB_NC, -1, spi, SPISettings()); //Lidt ærgerligt men vi bruger NEW
    RFM96 RFM = new Module(CS1, -1, RADIOLIB_NC, -1, spi, SPISettings());
    public:
    bool begin();
    float get_RSSI();
    bool transmit();
    int available();
    int setRX();
};

extern loRa_spi LoRa_spi;