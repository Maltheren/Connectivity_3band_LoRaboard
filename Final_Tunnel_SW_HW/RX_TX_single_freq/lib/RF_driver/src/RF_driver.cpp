#include "RF_driver.h"


loRa_spi LoRa_spi;


///De frekvenser vi arbejder med her.
const float RFM_FREQ = 433.0;
const float LBAA_FREQ1 = 2450.0;
const float LBAA_FREQ2 = 868.0;



bool loRa_spi::begin(float bw, uint8_t sf){
    pinMode(CS2, OUTPUT);
    pinMode(CS1, OUTPUT);
    digitalWrite(CS2, HIGH);
    digitalWrite(CS1, HIGH);

    SPI.begin(RF_SCK, RF_MISO, RF_MOSI);
    

    delay(1);
    int16_t result_UHF = RFM.begin(433, bw, sf, 7, RADIOLIB_LR11X0_LORA_SYNC_WORD_PRIVATE, 2 , 8, 0);
    delay(5);
    int16_t result_Sband = LBAA.begin(864.0,bw,sf,7,RADIOLIB_LR11X0_LORA_SYNC_WORD_PRIVATE,2,8,1.6);
    delay(5);
    SPI.setFrequency(10000000);  
    Serial.printf("Starting chips: \n \t [LBAA]: %d \n \t [RFM]: %d \n",result_Sband, result_UHF);
    
    //Sets permanent settings for the RFM module
    RFM.setFrequency(RFM_FREQ);
    RFM.setOutputPower(20); 
    float temp = 0;
    LBAA.getTemp(&temp);
    Serial.printf("Temperature is: %f", temp);

    
    return true;
}

bool loRa_spi::set_rx(){
    int state_LBAA = -1; //Igen lidt jank, vi skal bruge fejlkoderne flere gange
    int state_RFM = -1;

    state_LBAA = LBAA.startReceive();
    state_RFM = RFM.startReceive();
    Serial.printf("Setting RX [LBAA]: %d \t [RFM]: %d \n", state_LBAA,  state_RFM);
    return !(state_LBAA || state_RFM);
}



bool loRa_spi::poll(Measurement *output){
    float RSSI_LBAA = 0.0;
    float RSSI_RFM = 0.0;

    float SNR_LBAA = 0.0;
    float SNR_RFM = 0.0;

    int state_LBAA = -1; //Igen lidt jank, vi skal bruge fejlkoderne flere gange
    int state_RFM = -1;
    


    int16_t irq_status = LBAA.checkIrq(RADIOLIB_IRQ_RX_DONE);
    //Serial.printf("[LBAA][IRQ: %d]\n", irq_status);
    if(irq_status){ //Lidt janky men vi kigger om der ligger noget godt til os i bufferen
        //Vi modtaget noget godt fra LBAA'en
        //state_LBAA = LBAA.readData(bufferA, sizeof(bufferA));
        //Serial.print("packet: ");
        RSSI_LBAA = LBAA.getRSSI();
        SNR_LBAA = LBAA.getSNR();
        bufferA[0] = 0; //DIY semaforerer hehe
        //Serial.printf("[LBAA]: \t SNR: %f \t RSSI %f \n", SNR_LBAA, RSSI_LBAA);
        if(state == SBAND){
            output->freq = (uint32_t)LBAA_FREQ1;
        }
        else{
            output->freq = (uint32_t)LBAA_FREQ2;
        }

        output->RSSI = RSSI_LBAA;
        output->SNR = SNR_LBAA;
        LBAA.clearIrq(RADIOLIB_IRQ_RX_DEFAULT_FLAGS);
        return true;
    }

    int16_t RFM_irq = RFM.checkIrq(RADIOLIB_IRQ_RX_DONE); //Serial.printf("[RFM][IRQ: %d]\n", irq_status);
    if(RFM_irq){ //Igen lidt janky men vi kan se det på første byte
        //Vi har modtaget noget fra RFM'en
        //state_RFM = RFM.readData(bufferB, sizeof(bufferB));
        
        //Serial.print("packet: ");
        RSSI_RFM = RFM.getRSSI();
        SNR_RFM = RFM.getSNR();
        bufferB[0] = 0; //DIY semaforerer
        //Serial.printf("[RFM]: \t SNR: %f \t RSSI %f \n", SNR_RFM, RSSI_RFM);
        output->freq = (uint32_t)RFM_FREQ;
        output->RSSI = RSSI_RFM;
        output->SNR = SNR_RFM;
        RFM.clearIrq(RADIOLIB_IRQ_RX_DEFAULT_FLAGS);
        return true;
    }
    return false;
}


/// @brief Sets the settings for the next coming cycle such they are directly ready to transmit.
/// @param ID 
void loRa_spi::setup_settings_tx(Freq_settings Settings){
    int state_LBAA = -1; //Igen lidt jank, vi skal bruge fejlkoderne flere gange
    int state_RFM = -1;
    state = Settings;
    switch (state)
    {
    case SBAND:
        Serial.printf("MODE1 [TX] - %0.1f & %0.1f \n", RFM_FREQ, LBAA_FREQ1);
        digitalWrite(9, HIGH);
        LBAA.setFrequency(LBAA_FREQ1);
        LBAA.setOutputPower(20);
        digitalWrite(9, LOW);
        break;
    case UHF:
        Serial.printf("MODE2 [TX] - %0.1f \n", LBAA_FREQ2);
        digitalWrite(9, HIGH);
        LBAA.setFrequency(LBAA_FREQ2);
        LBAA.setOutputPower(20);
        digitalWrite(9, LOW);
        break;
    default:
        break;
    }
}




const char test[4] = {3, 2, 1, 0};
void loRa_spi::transmit_all(){
    int state_LBAA = -1; //Igen lidt jank, vi skal bruge fejlkoderne flere gange
    int state_RFM = -1;

    //Serial.printf("Expected time on air: [LBAA: %ld us], [RFM: %ld us]", LBAA.getTimeOnAir(sizeof(test)), RFM.getTimeOnAir(sizeof(test)));
    switch (state) //Vi skal sende enten i state 1 eller 0
    {
    case SBAND:
        /* code */
        state_LBAA = LBAA.startTransmit(test, sizeof(test));
        delay(5);
        state_RFM = RFM.startTransmit(test, sizeof(test));
        delay(5);
        //uint8_t cmd[5] = {3, 2, 1, 0};
        /*
        digitalWrite(CS2,LOW);
        SPI.write16(RADIOLIB_LR11X0_CMD_WRITE_BUFFER);
        SPI.transfer(cmd ,sizeof(cmd));
        digitalWrite(CS2, HIGH);
        */
            

        break;
    case UHF:

        
        state_LBAA = LBAA.startTransmit(test);
        delay(5);
        state_RFM = RFM.startTransmit(test);
        delay(5);
        
        break;

    default:
        Serial.print("Transmitter Undefined state");
        break;
    }
    //En sød besked til dem der skal fejlfinde.

    Serial.printf("Transmitted: \t [LBAA: %d] \t [RFM: %d] \t \n", state_LBAA, state_RFM);
    

}



