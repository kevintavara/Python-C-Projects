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

signed int tempX = 1;
signed int tempY = 2;
signed int tempZ = -3;

signed int iX = 4;
signed int iY = -5;
signed int iZ = 6;

signed int bX = 7;
signed int bY = -8;
signed int bZ = 9;

signed int X = 0;
signed int Y = 0;
signed int Z = 0;;
signed int Acc = 0;


int getTempX(void) {
  return tempX;
}

int getTempY(void) {
  return tempY;
}

int getTempZ(void) {
  return tempZ;
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
  return bX;
}

int getMagFieldY(void) {
  return bY;
}

int getMagFieldZ(void) {
  return bZ;
}

void setX(int x) {
  X = (int)x;
  printf("%d\n", X);
}

void setY(int y) {
  Y = (int)y;
  printf("%d\n",Y);
}

void setZ(int z) {
  Z = (int)z;
  printf("%d\n",Z);
}

void setAcc(int acc) {
	Acc = (int)acc;
	printf("%d\n",Acc);
}
