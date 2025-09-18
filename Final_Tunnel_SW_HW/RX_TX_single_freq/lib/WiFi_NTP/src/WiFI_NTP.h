#include "WiFi.h"
#include "WiFiUdp.h"
#include "NTPClient.h"

/* =================== BLIVER IKKE BRUGT =============
 * 
 *  Den her var originalt til at time multiplexe med Lora modulerne. men det blev droppet.
 * 
 * 
 */



/// ========== Settings =========
#define SSID "Test"
#define PSWD "TestTest"

extern WiFiUDP ntpUDP;
#ifndef SYNC_HANDLER
#define SYNC_HANDLER
enum timer_state {send, recieve, none};

class sync_handler{

    private:
    
    void (*timer_func)(int64_t*);
    int64_t time_start;
    int64_t period;
    int64_t overlap = 0;
    timer_state state = none;
    int64_t timeout_temp = 0;

    public:
    void connect();
    void sync();
    int64_t get_time_us();
    time_t get_epoch();


    void set_overlap(int64_t overlap);


    void arm_transmit(void (*called_func)(int64_t* timeout), time_t start, int64_t period);
    void arm_recieve(void (*called_func)(int64_t* timeout), time_t start, int64_t period);
    void update_loop();
};

timeval hms2timeval(uint32_t hr, uint32_t min, uint32_t second);
#endif

extern sync_handler Sync_handler;
