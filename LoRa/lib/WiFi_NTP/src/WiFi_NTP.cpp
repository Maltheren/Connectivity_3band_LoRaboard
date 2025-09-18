#include "WiFI_NTP.h"



WiFiUDP ntpUDP;

//ingen grund til at gøre livet hårdt for os selv vi tager en ntp tæt på

NTPClient timeClient(ntpUDP, "dk.pool.ntp.org", 0, 60000);;

sync_handler Sync_handler;



void sync_handler::connect(){
    
    Serial.println("Waiting for WiFi to sync RTC");
    Serial.print("Wifis in range: ");
    Serial.println(WiFi.scanNetworks());
    WiFi.mode(WIFI_STA);
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


void sync_handler::sync(){
    timeClient.begin(); //Hopper på en NTP pool.
    bool sucess = timeClient.forceUpdate();
    Serial.print("Force updated from NTP pool: ");
    
    
    Serial.println(timeClient.getFormattedTime());


    //Henter sekunder efter 1970
    time_t epoch_now = timeClient.getEpochTime();

    //Nu går der lidt hacking i den. Men vi venter på sekundskiftet
    while(epoch_now == timeClient.getEpochTime()){};

    //Og så sprinter vi den værdi in i vores system time.
    timeval tv;
    tv.tv_sec = epoch_now+1; // Already next second
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