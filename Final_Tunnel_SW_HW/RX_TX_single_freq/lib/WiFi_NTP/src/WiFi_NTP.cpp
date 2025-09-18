#include "WiFI_NTP.h"



WiFiUDP ntpUDP;

//ingen grund til at gøre livet hårdt for os selv vi tager en ntp tæt på

NTPClient timeClient(ntpUDP, "dk.pool.ntp.org", 0, 60000);;

sync_handler Sync_handler;



void sync_handler::connect(){
    
    Serial.println("Waiting for WiFi to sync RTC");
    //Serial.print("Wifis in range: ");
    //Serial.println(WiFi.scanNetworks());
    WiFi.mode(WIFI_STA);
    WiFi.setHostname("LoRa_3band_boards_CNMS");
    WiFi.begin(SSID, PSWD);
    while(WL_CONNECTED != WiFi.status()){
        Serial.print(".");
        delay(500);
        if(WiFi.status() == WL_CONNECT_FAILED){
            WiFi.begin(SSID, PSWD);
        }
    }
    

    Serial.print("Connected at: ");
    Serial.println(WiFi.localIP());


}

timeval hms2timeval(uint32_t hr, uint32_t min, uint32_t second){
    timeval time;
    time.tv_sec = hr * 3600 + min * 60 + second;
    time.tv_usec = 0;
    return time;
}


void sync_handler::update_loop(){
    if(state == none){
        return;
    }
    
    int64_t now = get_time_us();

    if(now > time_start){
        time_start += period;
        
        (*timer_func)(&time_start);  // Kalder vores timerfunktion
    }
    //og så går livet videre
}




void sync_handler::set_overlap(int64_t overlap){ //overlap i millisekunder
    this->overlap = overlap*1000; //VI skal lige konvertere til mikrosekunder.
}

/// @brief Sætter en funktion op med konstant tid der kalder vore calledfunc, start i tidspunktet det skal starte fra, periode er hvor ofte i millisekunder det skal foregå i.
/// @param called_func 
/// @param start 
/// @param period 
void sync_handler::arm_transmit(void (*called_func)(int64_t* timeout), time_t start, int64_t period){ //periodetid i millisekunder
    Serial.print("mode transmit Starting at ");
    Serial.println(ctime(&start));
    uint64_t t_start_us = ((uint64_t)start)*1000000UL;
    timer_func = called_func;
    this->period = period*1000;
    time_start = t_start_us;
    state = send;
}
void sync_handler::arm_recieve(void (*called_func)(int64_t* timeout), time_t start, int64_t period){
    Serial.print("mode recieve Starting at ");
    Serial.println(ctime(&start));
    uint64_t t_start_us = ((uint64_t)start)*1000000UL;
    
    timer_func = called_func;
    this->period = period*1000; //konverterer ms til us 
    time_start = t_start_us; //tidspunkt vi skal starte
    state = recieve;
}



void sync_handler::sync(){

    timeClient.begin(); //Hopper på en NTP pool.
    Serial.print("Force updating from NTP pool: ");
    bool sucess = timeClient.forceUpdate();



    
    
    Serial.println(timeClient.getFormattedTime());
    

    //Henter sekunder efter 1970
    time_t epoch_now = timeClient.getEpochTime();

    //Nu går der lidt hacking i den. Men vi venter på sekundskiftet
    while(epoch_now == timeClient.getEpochTime()){};

    //Og så sprinter vi den værdi in i vores system time.
    timeval tv;
    tv.tv_sec = epoch_now+1 + 2*3600; // Already next second
    tv.tv_usec = 0;
    settimeofday(&tv, nullptr);
    
    Serial.print("Systime: ");
    
    timeval tester;
    gettimeofday(&tester, nullptr);

    timeClient.end();
    Serial.print("Systtime");
    Serial.print(ctime(&tester.tv_sec));


    
}

//Henter tiden i mikrosekunder
int64_t sync_handler::get_time_us(){
    timeval tv_now;
    gettimeofday(&tv_now, NULL);
    int64_t time_us = (int64_t)tv_now.tv_sec * 1000000L + (int64_t)tv_now.tv_usec;
    return time_us;
}

//Henter vore tid i epoch
time_t sync_handler::get_epoch(){
    timeval tv_now;
    gettimeofday(&tv_now, nullptr);
    return tv_now.tv_sec; 
}