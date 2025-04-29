

#include <Arduino.h>
#include <SD_driver.h>
#include <WiFI_NTP.h>
#include <RF_driver.h>


#define RF_mosi     17
#define RF_miso     16
#define RF_SCK      18





void setup() {
  esp_log_level_set("*", ESP_LOG_VERBOSE);
  Serial.begin(115200);
  //pinMode(RF_mosi, OUTPUT);
  //pinMode(RF_miso, OUTPUT);
  //pinMode(RF_SCK, OUTPUT);
  Sd_driver.begin();
  pinMode(SD_detect, INPUT);
  Serial.print("We are alive");
  LoRa_spi.begin();
  //Sync_handler.connect();
  //Sync_handler.sync();
  /*
  while (!Sd_driver.inserted()){
    Serial.println("SD not inserted");
    delay(200);
  }
  Serial.println("Trying to mount");
  while(!Sd_driver.mount()){
    Serial.print(".");
    delay(200);
  }
  */

}




void loop() {
  //time_t test = Sync_handler.get_epoch();

  //Serial.printf("Human time: %s Time us: %lld \n",ctime(&test), (int64_t)Sync_handler.get_time_us());
  
  delay(500);

}


