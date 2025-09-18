

#include <Arduino.h>
#include <SD_driver.h>
#include <WiFI_NTP.h>
#include <RF_driver.h>
#include <LoRa.h>



SPIClass spi = SPIClass(FSPI);

//RFM96 rfm96w = new Module(CS1, RADIOLIB_NC, RADIOLIB_NC, RADIOLIB_NC, spi);




void setup() {
  pinMode(CS2, OUTPUT);
  pinMode(CS1, OUTPUT);
  digitalWrite(CS2, HIGH);
  digitalWrite(CS1, HIGH);
  spi.begin(RF_SCK, RF_MISO, RF_MOSI);

  Serial.begin(115200);
  LoRa.setSPI(spi);
  LoRa.setPins(CS1,-1,-1);
  
  if(!LoRa.begin(434E6)){
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  Serial.print("Alive");

  
}




int counter = 0;
void loop() { 
  Serial.println("ping");
  // send packet
  LoRa.beginPacket();
  LoRa.print("hello ");
  LoRa.print(counter);
  LoRa.endPacket();
  counter++;
  delay(1000);

}


