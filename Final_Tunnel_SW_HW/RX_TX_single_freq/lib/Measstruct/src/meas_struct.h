#include <Arduino.h>
#ifndef Struct_meas
#define Struct_meas
struct Measurement{
    public:
    uint32_t freq; //Frekvens i MHz
    float RSSI; //dB
    float SNR;  //Linear
    uint32_t ID;  //Noget vi kan grave frem senere
};
#endif