///ESP32 og FREERTOS libs
#include <esp32/clk.h>


//Custom libs
#include <Arduino.h>
#include <SD_driver.h>
#include <RF_driver.h>
#include <WiFI_NTP.h>

//SKetchy log funktion
#define LOG(fmt, ...) \
    Serial.printf("[%s] " fmt "\n", pcTaskGetName(NULL), ##__VA_ARGS__)

#define TXLED 9
#define MISCLED 10
#define switch_frequency 11

// Defines for om vi skal være i RX eller TX mode, vi kører det samlet for ligesom at sikre os a LoRa deler indstillinger
#define mode_RX
#define mode_TX
//Define hvilket frekvensbånd vi skal køre på

//#define RF_mode UHF



SemaphoreHandle_t blink;
void task_blinkled(void* parameters){

  while(true){
    xSemaphoreTake(blink, portMAX_DELAY);
    digitalWrite(TXLED, HIGH);
    delay(60);
    digitalWrite(TXLED, LOW);
  }
}

#ifdef mode_TX


#endif

#ifdef mode_RX
//RTOS
void task_sd_logger(void* parameters); //Prototype, findes derude.
QueueHandle_t Measurement_queue;

#endif


volatile int RF_pin = 0;


void setup() {
  Serial.begin(115200);
  Serial.print("We are alive");
  Serial.printf("CPU FREQ: %ld \n", esp_clk_cpu_freq()); //For at være sikker på den rigtige frekvens
  LoRa_spi.begin(500.0, 6);



  blink = xSemaphoreCreateBinary();
  TaskHandle_t xhandle_blinkled = NULL;

  xTaskCreatePinnedToCore(
  task_blinkled,  /* Task function. */
  "BLinker",    /* name of task. */
  10000,      /* Stack size of task */
  NULL,       /* parameter of the task */
  2,          /* priority of the task */
  &xhandle_blinkled,     /* Task handle to keep track of created task */
  0);         /* pin task to core 0 */  


  pinMode(switch_frequency, INPUT_PULLUP);
  pinMode(TXLED, OUTPUT);
  pinMode(MISCLED, OUTPUT);
  
  Freq_settings settings = SBAND;
  delay(100);
  RF_pin = digitalRead(switch_frequency);
  if(RF_pin){
    settings = UHF;
  }
  LoRa_spi.setup_settings_tx(settings); //den her sætter hvad frekvevnser vi kører på der er UHF = (433, 868) og SBAND=(433, 2450)


  #ifdef mode_TX



  #endif
  #ifdef mode_RX

  Sync_handler.connect();
  Sync_handler.sync();
  time_t epoch = Sync_handler.get_epoch();
  Serial.printf("Setting RX [RF_DRIVER: %d]", LoRa_spi.set_rx());


  //RTOS ting og sager

  TaskHandle_t xhandle_logger = NULL;

  xTaskCreatePinnedToCore(
  task_sd_logger,  /* Task function. */
  "LOGGER",    /* name of task. */
  10000,      /* Stack size of task */
  NULL,       /* parameter of the task */
  2,          /* priority of the task */
  &xhandle_logger,     /* Task handle to keep track of created task */
  0);         /* pin task to core 0 */  

  Measurement_queue = xQueueCreate(25, sizeof(Measurement)); //Køen ti vores målinger der popper ind fra radioen.

  #endif
}


Measurement temp = {0, 0.0, 0.0, 255};




void loop() {
    // put your main code here, to run repeatedly:
    #ifdef mode_TX
    delay(10); 
    unsigned long t_start = millis();

    LoRa_spi.transmit_all(); 

    xSemaphoreGive(blink); 

    
    #endif

    #ifdef mode_RX
    delay(10);
    uint32_t t_start = millis();
    if(LoRa_spi.poll(&temp)){
      if(xQueueSend(Measurement_queue,&temp, 100) != pdPASS){
        LOG("Queue failed");
      }
      xSemaphoreGive(blink); //For debug så blinker vi
    }

    #endif
    if(digitalRead(switch_frequency) != RF_pin){ //Hvis vi nu hellere vil køre på UHF eller SBAND undervejs
      ESP.restart();
    }
}

#ifdef mode_RX
/// @brief Task to handle the SD_card logger
void task_sd_logger(void* parameters){
  Sd_driver.begin();
  LOG("Insert SD");
  while(!Sd_driver.inserted()){
    delay(100);
  }
  if(Sd_driver.mount()){
    LOG("SD OK");
  }
  else{
    LOG("Mounting failed try again");
  }


  time_t t_now = Sync_handler.get_epoch();
  String now = String(ctime(&t_now));
  now.trim(); // removes leading/trailing whitespace including newline
  now.replace(" ", "_");
  now.replace(":", "_");
  now.concat(".csv");
  Sd_driver.setup_log(now, "");


  //Lidt manuel timekeeping for at vi får autosavet
  uint32_t t_last = pdTICKS_TO_MS(xTaskGetTickCount());
  uint32_t period = 5000;

  Measurement current_meas = {0,0.0,0.0,255};//skulle have lavet en constructor til den her basse


  while(true){
    if(xQueueReceive(Measurement_queue, &current_meas, portMAX_DELAY) != pdPASS){ 
      LOG("Queue error");
    }
    Serial.printf("SNR: %0.2f \t RSSI: %0.1f \t FREQ: %d \n", current_meas.SNR, current_meas.RSSI, current_meas.freq);
    int64_t epoch_ms = Sync_handler.get_time_us()/1000l; //Hapser tiden i millisekunder
    time_t t_now = epoch_ms/1000l;
    char buf[128];
    char* timeStr = ctime(&t_now);  // returns something like "Tue May 21 15:12:00 2024\n"

    
    timeStr[strcspn(timeStr, "\n")] = 0;

    int length = snprintf(buf, sizeof(buf), "%s \t %03lld, \t %d, \t %.4f, \t %.4f \n",
            timeStr, (epoch_ms % 1000l), current_meas.freq, (double)current_meas.RSSI, (double)current_meas.SNR);
    Sd_driver.log(buf, length);
    //Serial.print(buf);
    if(t_last < pdTICKS_TO_MS(xTaskGetTickCount())){
      //LOG("autosave");
      t_last += period;
      Sd_driver.flush();
    }

  }
}
#endif