#include "FS.h"
#include "SD.h"
#include "SPI.h"


#define SD_detect   4
#define SD_DO       5
#define SD_DI       7
#define SD_SCK      6
#define SD_CS       15



class sd_driver{
    private:
    SPIClass spi = SPIClass(HSPI);
    File current_log;

    public:
    void begin();
    bool inserted();
    bool mount();
    bool setup_log(String name, String notes);
    void log(char* input, size_t size);
    void flush();
};

extern sd_driver Sd_driver;
