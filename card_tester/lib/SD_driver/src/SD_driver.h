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

    public:
    void begin();
    bool inserted();
    bool mount();

    
};

extern sd_driver Sd_driver;
