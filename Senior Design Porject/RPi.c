#include <errno.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <wiringPi.h>
#include <wiringSerial.h>
#include <time.h>
// Magnetometer Setup
//  Pin 1 3.3 to Red
//  Pin 3 SDA to Blue
//  Pin 5 SCL to Yellow
//  Pin 9 GND to Black
// UART Setup
//  6 GND to GND (MSP430)
//  Pin 8 (TX) to P2.1 (RX)
//  Pin 10 (RX) to P2.0 (TX)

unsigned char tempX = 1;
unsigned char tempY = 2;
unsigned char tempZ = 3;

unsigned char iX = 4;
unsigned char iY = 5;
unsigned char iZ = 6;

unsigned char mX = 7;
unsigned char mY = 8;
unsigned char mZ = 9;


char getTempX() {
  return tempX;
}

char getTempY() {
  return tempY;
}

char getTempZ() {
  return tempZ;
}

char getCurrentX() {
  return iX;
}

char getCurrentY() {
  return iY;
}

char getCurrentZ() {
  return iZ;
}

void setX(unsigned char x) {
  mX = x;
  printf(mX);
}

void setY(unsigned char x) {
  mY = x;
  printf(mY);
}

void setZ(unsigned char x) {
  mZ = x;
  printf(mZ);
}

int main ()
{
  int fd;
  int file;
  char *bus = "/dev/i2c-1";
  char RX;
  char TX = 0xD1;
  /* open serial port */
  if ((fd = serialOpen ("/dev/ttyS0", 9600)) < 0) {
    fprintf (stderr, "Unable to open serial device: %s\n", strerror (errno)) ;
    return 1;
  }
  /* initializes wiringPi setup */
  if (wiringPiSetup () == -1) {
    fprintf (stdout, "Unable to start wiringPi: %s\n", strerror (errno)) ;
    return 1;
  }
  // Create I2C bus
  if((file = open(bus, O_RDWR)) < 0) {
    printf("Failed to open the bus. \n");
    return 1;
  }
  // Get I2C device, MLX30393 I2C address is 0x0C(12)
  ioctl(file, I2C_SLAVE, 0x0C);

  while(1) {
	// Write register command, AH = 0x00, AL = 0x5C, Hall plate spinning rate = DEFAULT, GAIN_SEL = 5(0x60, 0x00, 0x5C, 0x00)
	// Address register, (0x00 << 2)
	char config[4] = {0};
	config[0] = 0x60;
	config[1] = 0x00;
	config[2] = 0x5C;
	config[3] = 0x00;
	write(file, config, 4);
	// Read 1 Byte of data
	// status byte
	char data[7] = {0};
	read(file, data, 1);
	// Write register command, AH = 0x02, AL = 0xB4, RES for magnetic measurement = 0(0x60, 0x02, 0xB4, 0x08)
	// Address register, (0x02 << 2)
	config[0] = 0x60;
	config[1] = 0x02;
	config[2] = 0xB4;
	config[3] = 0x08;
	write(file, config, 4);
	// Read 1 Byte of data
	// status byte
	read(file, data, 1);
	// Start single meaurement mode, X, Y, Z-Axis enabled(0x3E)
	config[0] = 0x3E;
	write(file, config, 1);
	// Read 1 byte
	// status
	read(file, data, 1);
	sleep(1);
	// Send read measurement command, X, Y, Z-Axis enabled(0x4E)
	config[0] = 0x4E;
	write(file, config, 1);

	// Read 7 bytes of data
	// status, xMag msb, xMag lsb, yMag msb, yMag lsb, zMag msb, zMag lsb
	if(read(file, data, 7) != 7) {
		printf("Error : Input/Output error \n");
	}
	else {
		// Convert the data
		int xMag = (data[1]* 256 + data[2]);
		if(xMag > 32767) {
			xMag -= 65536;
		}
		int yMag = (data[3] * 256 + data[4]);
		if(yMag > 32767) {
			yMag -= 65536;
		}
		int zMag = (data[5] * 256 + data[6]);
		if(zMag > 32767) {
			zMag -= 65536;
		}

		// Output data to screen
		printf("Magnetic Field in X-Axis : %d \n", xMag);
		printf("Magnetic Field in Y-Axis : %d \n", yMag);
		printf("Magnetic Field in Z-Axis : %d \n\n", zMag);
	}
        
	// 0xF9 to be transmitted to RPi
	// 0xF9 can be changed to anything if 
	// We want to transmit a different value
	serialPutchar(fd, TX);
	
	// Here we check if data is available
	if(serialDataAvail (fd) ) { 
	  // Here we store the data from the MSP430
	  RX = serialGetchar(fd);
	  printf ("%3d\n", RX);
	  fflush (stdout) ;
	} 
	fflush (stdout) ;
  }

}