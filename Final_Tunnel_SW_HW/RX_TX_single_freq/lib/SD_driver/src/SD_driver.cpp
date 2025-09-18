#include<SD_driver.h>



sd_driver Sd_driver;

/// @brief Sætter den pin up der tjekker insert switchen. Det bare så vi kan se om der er et SD kort i
void sd_driver::begin(){
    pinMode(SD_detect, INPUT);
    
    spi.begin(SD_SCK,SD_DO,SD_DI);
}

/// @brief TJekker om der er er SD kort i
/// @return Bool om der er et kort tilstede
bool sd_driver::inserted(){
    return !digitalRead(SD_detect);
}


/// @brief Forsøger at initialisere et kort, ved at snakke med det.
/// @return 
bool sd_driver::mount(){

    bool mount_res = SD.begin(SD_CS, spi);
    if(!mount_res) return mount_res;
    SD.mkdir("/logs");

    
    Serial.print("created FOlder");
    return mount_res;
}

bool sd_driver::setup_log(String name, String notes){

    current_log = SD.open("/logs/"+name, "a",true);
    Serial.print("Current log: \t /logs/"+name);
    if(notes.length()){
        current_log.println(notes);
    }   
    

    return true;
}

void sd_driver::flush(){
    current_log.flush();
}


void sd_driver::log(char* input, size_t size){
    current_log.write((uint8_t*)input, size);
}