#include<SD_driver.h>



sd_driver Sd_driver;

/// @brief Sætter den pin up der tjekker RF switchen. Det bare så vi kan se om der er et SD kort i
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
    SD.mkdir("/testeste");
    File testfile = SD.open("/testeste/test.txt", "wr",true);
    testfile.println("SKRRRRRRT");

    testfile.println("SKRRRRRRT2");
    testfile.close();
    return mount_res;
}