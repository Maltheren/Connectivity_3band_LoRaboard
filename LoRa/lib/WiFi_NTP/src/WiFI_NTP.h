#include "WiFi.h"
#include "WiFiUdp.h"
#include "NTPClient.h"


/// ========== Settings =========
#define SSID "Test"
#define PSWD "TestTest"

extern WiFiUDP ntpUDP;



class sync_handler{

    
    public:
    
    void connect();
    void sync();
    int64_t get_time_us();
    time_t get_epoch();
};


extern sync_handler Sync_handler;