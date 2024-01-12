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
#include <stdbool.h>
// Magnetometer Setup
//  Pin 1 3.3 to Red
//  Pin 3 SDA to Blue
//  Pin 5 SCL to Yellow
//  Pin 9 GND to Black
// UART Setup
//  6 GND to GND (MSP430)
//  Pin 8 (TX) to P2.1 (RX)
//  Pin 10 (RX) to P2.0 (TX)
bool stop = false;
bool cal = true;
bool Static = false;
bool status = true;
bool Dynamic = false;
int mAcc = 0;
int mX = 0;
int mY = 0;
int mZ = 0;
bool x = false;
bool y = false;
bool z = false;
unsigned char iX = 1;
unsigned char iY = 2;
unsigned char iZ = 3;
signed int dX = 0;
signed int dY = 0;
signed int dZ = 0;
signed int tX = 4;
signed int tY = 5;
signed int tZ = 6;
signed int currentX = 100;
signed int currentY = 100;
signed int currentZ = 100;
void calibrate(int X, int Y, int Z);
void simulate(int X, int Y, int Z, int Xmag, int Ymag, int Zmag);

int getTempX(void) {
  return tX;
}

int getTempY(void) {
  return tY;
}

int getTempZ(void) {
  return tZ;
}

int getCurrentX(void) {
  return iX;
}

int getCurrentY(void) {
  return iY;
}

int getCurrentZ(void) {
  return iZ;
}

int getMagFieldX(void) {
  return currentX;
}

int getMagFieldY(void) {
  return currentY;
}

int getMagFieldZ(void) {
  return currentZ;
}

void setStatus(bool s){
  status = s;
}

void setCal(bool c){
  cal = c;
}

void setStatic(bool stat){
  Static = stat;
}

void setDynamic(bool d){
  Dynamic = d;
}

bool getProgress(void){
  return !cal;
}

void setX(int x) {
  dX = (int)x;
  printf("%d\n", mX);
}

void setY(int y) {
  dY = (int)y;
  printf("%d\n",mY);
}

void setZ(int z) {
  dZ = (int)z;
  printf("%d\n",mZ);
}

void setAcc(int acc) {
  mAcc = (int)acc;
  printf("%d\n",mAcc);
}

int main () {
  int fd;
  int file;
  char *bus = "/dev/i2c-1";

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
  
  while(status) {
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
		//printf("Error : Input/Output error \n");
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
		printf("Magnetic Field in X-Axis : %d [uT]\n", xMag);
		printf("Magnetic Field in Y-Axis : %d [uT]\n", yMag);
		printf("Magnetic Field in Z-Axis : %d [uT]\n\n", zMag);
		currentX = xMag;
		currentY = yMag;
		currentZ = zMag;
	}
	// Transmit duty cycles to MSP430
	serialPutchar(fd, mX);
	serialPutchar(fd, mY);
	serialPutchar(fd, mZ);
	
	// Receive data from MSP430
	if (serialDataAvail(fd)) {
	  iX = serialGetchar(fd);
	  iY = serialGetchar(fd);
	  iZ = serialGetchar(fd);
	  tX = serialGetchar(fd);
	  tY = serialGetchar(fd);
	  tZ = serialGetchar(fd);
	  
	}
	
	printf ("Current X: %d\n",iX);
	printf ("Current Y: %d\n",iY);
	printf ("Current Z: %d\n",iZ);
	
	printf ("Temperature X: %d\n", tX);
	printf ("Temperature Y: %d\n", tY);
	printf ("Temperature Z: %d\n", tZ);
	
	
	// Check if calibration on
	if(cal)
	  calibrate(currentX, currentY, currentZ);
        if(Dynamic) {
	  
	  simulate(currentX, currentY, currentZ, dX, dY, dZ);
	}
	if(Static) {
	  simulate(currentX, currentY, currentZ, dX, dY, dZ);
	}
	// Check if stop enabled
	if(stop) {
	  mX = 0;
	  mY = 0;
	  mZ = 0;
	}
	 
    }

}

void simulate(int X, int Y, int Z, int Xmag, int Ymag, int Zmag) {
    if(X > Xmag + 5) {
      mX--;
    }
    else if(X < Xmag - 5) {
      mX++;
    }
    else
      x = true;
    if(Y > Ymag + 15) {
      mY--;
    }
    else if(Y < Ymag - 10) {
      mY++;
    }
    else
      y = true;
    if(Z > Zmag + 15) {
      mZ--;
    }
    else if(Z < Zmag - 10) {
      mZ++;
    }
    else 
      z = true;
    
    if (x && y && z && Dynamic) {
      z = false;
      y = false;
      z = false;
      printf("Dynamic Simulation Iteration Complete!\n");
    } 
    if (x && y && z && Static) {
      Static = false
      z = false;
      y = false;
      z = false;
      printf("Static Simulation Complete!\n");
    } 
}

void calibrate(int X, int Y, int Z) {
    if(X > 10) {
      mX--;
    }
    else if(X < -10) {
      mX++;
    }
    else 
      x = true;
    if(Y > 10) {
      mY--;
    }
    else if(Y < -10) {
      mY++;
    }
    else
      y = true;
    if(Z > 10) {
      mZ--;
    }
    else if(-10 > Z) {
      mZ++;
    }
    else 
      z = true;
    
    if (x && y && z) {
      cal = false;
      z = false;
      y = false;
      z = false;
      printf("Calibrated!\n");
    } 
}
