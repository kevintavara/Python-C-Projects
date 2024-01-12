#include <stdio.h>
#include <wiringPiSPI.h>
#define SPI_CHANNEL 0
#define SPI_CLOCK_SPEED 100000
#define SPI_MODE 3

int main(int argc, char **argv)
{
    int fd = wiringPiSPISetupMode(SPI_CHANNEL, SPI_CLOCK_SPEED, SPI_MODE);

    if (fd == -1) {
        printf("Failed to init SPI communication.\n");
        return -1;
    }

    printf("SPI communication successfully setup.\n");
   
    unsigned char buf[2] = {0x04, 0x05};

    wiringPiSPIDataRW(SPI_CHANNEL, buf, 2);

    printf("Data returned: %s\n", buf[1]); 

    return 0;
}
