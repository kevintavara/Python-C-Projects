#include <stdio.h>
#include <wiringPiI2C.h>
#include <wiringPi.h>

#define I2C_ID 0x48

int main (int argc, char **argv)
{
    // Setup I2C communication
    int fd = wiringPiI2CSetup(I2C_ID);

    if (fd == -1) {
        printf("Failed to init I2C communication.\n");
        return -1;
    }

    printf("I2C communication successfully setup.\n");

    while (1) {
 	wiringPiI2CWrite(fd, 0xEF);
	delay(500);
 	wiringPiI2CWrite(fd, 0xFF);
	delay(500);
    }
    return 0;
}