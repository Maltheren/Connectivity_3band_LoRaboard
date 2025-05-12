

#include <Arduino.h>
#include <SD_driver.h>
#include <WiFI_NTP.h>
#include <RF_driver.h>


void reciever_state(int64_t* timeout){
  digitalWrite(10, HIGH);
  int64_t t_stop = (*timeout-10000);
  LoRa_spi.setRX();
  while(true){ //Hvis vi timeouter
    if(Sync_handler.get_time_us() > t_stop){
      digitalWrite(10, LOW);
      Serial.print("Timeout!!");

      return;
    }
    

    if(LoRa_spi.available()){
      Serial.print("BESKED!!");
      break; //Vi skal ud og tolke på en pakke yay.
    }
    
    delay(10);
  }
  
  Serial.print("RSSI: ");
  Serial.print(LoRa_spi.get_RSSI());
  
  digitalWrite(10, LOW);

}



void transmitter_state(int64_t* timeout){
  Serial.print("PING");
  digitalWrite(10, HIGH);
  LoRa_spi.transmit();
  digitalWrite(10, LOW);
}

void setup() {
  esp_log_level_set("*", ESP_LOG_VERBOSE);
  Serial.begin(115200);
  pinMode(RF_MOSI, OUTPUT);
  pinMode(RF_MISO, OUTPUT);
  pinMode(RF_SCK, OUTPUT);


  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  //Sd_driver.begin();
  //pinMode(SD_detect, INPUT);
  Serial.print("We are alive");
  LoRa_spi.begin();
  //Sync_handler.connect();
  //Sync_handler.sync();
  //time_t epoch = Sync_handler.get_epoch();
  //time_t start_t = ((epoch+ 60)/ 60)*60;
  
  //Sync_handler.set_overlap(0);
  WiFi.disconnect(true);
  
  //#ifdef MODE_RECIEVE
  //Serial.print("RECIEVEMODE");
  //Sync_handler.arm_recieve(reciever_state, start_t, 1000); //Vi runder absolut op
  //LoRa_spi.setRX();
  //#endif


  //#ifdef MODE_SEND
  //Serial.print("SENDMODE");
  //Sync_handler.arm_transmit(transmitter_state, start_t, 1000); //Vi runder absolut op

  //#endif
  //Serial.print("Starting at: ");
  //Serial.print(ctime(&start_t)); 





}



void loop() {
  //Sync_handler.update_loop();
  Serial.print(LoRa_spi.setRX());
  delay(100);
  Serial.print(LoRa_spi.get_RSSI());
  Serial.println("");
  delay(100);
}


//Serial.printf("EPOCH: %d \n", start_t);
//Serial.printf("EPOCH NOW: %d", Sync_handler.get_epoch());
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
//pinMode(RF_MOSI, OUTPUT);