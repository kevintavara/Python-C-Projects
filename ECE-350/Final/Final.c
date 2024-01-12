#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <wiringPiI2C.h>
#include <wiringPiSPI.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>
#include <mcp3004.h> // For MCP3004 and MCP3008
#include <string.h>

#define PI 3.141592
#define X_REG 0x32
#define Y_REG 0x34
#define Z_REG 0x36
#define MAXTIMINGS 85
#define PIR_OUT_PIN 29
#define DHTPIN 25

int dht11_dat[5] = {0,0,0,0,0};
const int ROW[]    = {0, 1, 2, 3};
const int COLUMN[] = {4, 5, 6, 7};
short int axis_sample_average(int axis, int fd);
short int axis_sample(int axis,int fd);
char logH[1000];
char code[4];
char none[4];

typedef unsigned char uchar;

/*
 * File name   : dht11.c
 * Description : Acquire temperature and humidity.
 * Website     : www.adeept.com
 * E-mail      : support@adeept.com
 * Author      : Jason
 * Date        : 2015/06/21
 */
void read_dht11_dat()
{
	uint8_t laststate = HIGH;
	uint8_t counter = 0;
	uint8_t j = 0, i;
	float f;

	dht11_dat[0] = dht11_dat[1] = dht11_dat[2] = dht11_dat[3] = dht11_dat[4] = 0;

	// pull pin down for 18 milliseconds
	pinMode(DHTPIN, OUTPUT);
	digitalWrite(DHTPIN, LOW);
	delay(18);
	// then pull it up for 40 microseconds
	digitalWrite(DHTPIN, HIGH);
	delayMicroseconds(40); 
	// prepare to read the pin
	pinMode(DHTPIN, INPUT);

	// detect change and read data
	for ( i=0; i< MAXTIMINGS; i++) {
		counter = 0;
		while (digitalRead(DHTPIN) == laststate) {
			counter++;
			delayMicroseconds(1);
			if (counter == 255) {
				break;
			}
		}
		laststate = digitalRead(DHTPIN);

		if (counter == 255) break;

		// ignore first 3 transitions
		if ((i >= 4) && (i%2 == 0)) {
			// shove each bit into the storage bytes
			dht11_dat[j/8] <<= 1;
			if (counter > 16)
				dht11_dat[j/8] |= 1;
			j++;
		}
	}

	// check we read 40 bits (8bit x 5 ) + verify checksum in the last byte
	// print it out if data is good
	if ((j >= 40) || 
			(dht11_dat[4] == ((dht11_dat[0] + dht11_dat[1] + dht11_dat[2] + dht11_dat[3]) & 0xFF)) ) {
		f = dht11_dat[2] * 9. / 5. + 32;
		printf("Humidity = %d.%d %% Temperature = %d.%d *C (%.1f *F)\n", dht11_dat[0], dht11_dat[1], dht11_dat[2], dht11_dat[3], f);
	}
	else
	{
		printf("Data not good, skip\n");
	}
}

/*
 * File name   : adxl345.c
 * Description : Detect if adxl is being moved
 * Website     : www.adeept.com
 * E-mail      : support@adeept.com
 */
short int axis_sample(int axis,int df)
{
        short int data = 0;
        short int data2 = 0;

        data  =  wiringPiI2CReadReg8(df,axis);
        data2 =  wiringPiI2CReadReg8(df,axis+1);

        return ( (data2<<8)|data );
}
short int axis_sample_average(int axis, int df)
{
        int c = 10;
        int value = 0;

        while(c--){
                value += axis_sample(axis, df);
        }

        return ( value/10 );
}

/*
 * File name   : matrixKeyboard.c
 * Description : Read buttons being pressed
 * Website     : www.adeept.com
 * E-mail      : support@adeept.com
 */
int getKey(void)
{
	int i;
	int tmpRead;
	int rowVal = -1;
	int colVal = -1;
	char keyVal;

	for(i = 0; i < 4; i++){
		pinMode(COLUMN[i], OUTPUT);
		digitalWrite(COLUMN[i], LOW);
	}

	for(i = 0; i < 4; i++){
		pinMode(ROW[i], INPUT);
		pullUpDnControl(ROW[i], PUD_UP);
	}
	
	for(i = 0; i < 4; i++){
		tmpRead = digitalRead(ROW[i]);
		if(tmpRead == 0){
			rowVal = i;
		}
	}

	if(rowVal < 0 || rowVal > 3){
		return -1;
	}
	
	for(i = 0; i < 4; i++){
		pinMode(COLUMN[i], INPUT);
		pullUpDnControl(COLUMN[i], PUD_UP);
	}

	pinMode(ROW[rowVal], OUTPUT);
	digitalWrite(ROW[rowVal], LOW);

	for(i = 0; i < 4; i++){
		tmpRead = digitalRead(COLUMN[i]);
		if(tmpRead == 0){
			colVal = i;
		}
	}
	
	if(colVal < 0 || colVal > 3){
		return -1;
	}

	//printf("%d, %d\n", rowVal, colVal);
	switch(rowVal){
		case 0:
			switch(colVal){
				case 0: keyVal = 0; break; 
				case 1: keyVal = 1; break;
				case 2: keyVal = 2; break;
				case 3: keyVal = 3; break;
				default:
					break;
			}
			break;
		case 1:
			switch(colVal){
				case 0: keyVal = 4; break;
				case 1: keyVal = 5; break;
				case 2: keyVal = 6; break;
				case 3: keyVal = 7; break;
				default:
					break;
			}
			break;
		case 2:
			switch(colVal){
				case 0: keyVal = 8; break;
				case 1: keyVal = 9; break;
				case 2: keyVal = 10; break;
				case 3: keyVal = 11; break;
				default:
					break;
			}
			break;
		case 3:
			switch(colVal){
				case 0: keyVal = 12; break;
				case 1: keyVal = 13; break;
				case 2: keyVal = 14; break;
				case 3: keyVal = 15; break;
				default:
					break;
				
			}
			break;
		default: 
			break;
	}

	return keyVal;
}

int main (void)
{
	int df = 0;
    int setup, x, y, key, lock, hour, min, sec, i;
    double x1;
    short int data = 0;
    short int data2 = 0;
    int datasimple = 0;
    uchar adcVal;
    double accelX, accelY, accelZ;

	if(wiringPiSetup() == -1){
		printf("wiringPi setup failed !\n");
		exit(1);
	}
	
	// MCP3008 Setup from NovelDevices.co.uk
	if(wiringPiSPISetup(0, 500000) == -1){
		printf("wiringPiSPI setup failed !\n");
		exit(1);
	}
	mcp3004Setup(100,0);
	
	// ADXL Setup
    df = wiringPiI2CSetup(0x53);
    datasimple = wiringPiI2CReadReg8(df,0x31);
    wiringPiI2CWriteReg8(df,0x31,datasimple|0xb);
    wiringPiI2CWriteReg8(df,0x2d,0x08); //POWER_CTL
    // erase offset bits
    wiringPiI2CWriteReg8(df,0x1e, 0);
    wiringPiI2CWriteReg8(df,0x1f, 0);
    wiringPiI2CWriteReg8(df,0x20, 0);
    // calibrate X axis
    data = axis_sample_average(X_REG,df);
    wiringPiI2CWriteReg8(df,0x1e, 0);
    // calibrate Y axis
    data = axis_sample_average(Y_REG,df);
    wiringPiI2CWriteReg8(df,0x1f, 0);
    // calibrate Z axis
    data = axis_sample_average(Z_REG,df);
    wiringPiI2CWriteReg8(df,0x20, 0); 
        
	pinMode(PIR_OUT_PIN, INPUT);
	
	while (1) 
	{
		accelX = axis_sample_average(X_REG,df);
        accelY = axis_sample_average(Y_REG,df);
        accelZ = axis_sample_average(Z_REG,df);
        key = -1;
        time_t mytime;
        struct tm * timeinfo;
        time (&mytime);
        timeinfo = localtime(&mytime);
        min = timeinfo ->tm_min;
        sec = timeinfo ->tm_sec; 
		
		// Log Report of humidity and temperature hourly
		// Check whether day or night hourly
		if (min == 0 && sec == 0){
			read_dht11_dat();
			// Reads first channel
			adcVal = analogRead(100);
			if (adcVal > 100){
				printf("Day\n");
			}
			else {
				printf("Night\n");
			} 
			delay(800);
		}
		
		// Check PIR for motion
		if(1 == digitalRead(PIR_OUT_PIN)){
			printf("Movement detected!\n");	
		}
		
		// Check if code is being inputed
		key = getKey();
		if (key > -1){
			// Code is 1213
			// Lock is used as counter to track number of correct inputs
			if (key == 0 && lock == 0){
				code[1] = '1';
				lock = 1;
			}
			else if (key == 1 && lock == 1){
				code[2] = '2';
				lock = 2;
			}
			else if (key == 0 && lock == 2){
				code[3] = '1';
				lock = 3;
			}
			else if (key == 2 && lock == 3){
				code[4] = '3';
				lock = 0;
			}
			else {
				code[1] = '1';
				code[2] = '1';
				code[3] = '1';
				code[4] = '1';
				lock = 0;
				printf("Wrong code\n");
			}
		}
		// If code inputted correctly then reset values
		if (code[1] == '1' && code[2] == '2' && code[3] == '1' && code[4] == '3'){
			printf("Vault Door Opened! Code was entered.\n");
			code[1] = '1';
			code[2] = '1';
			code[3] = '1';
			code[4] = '1';
			lock = 0;
		}
		
		// Check if bust was moved
		if (accelX > 3 || accelX < -2){
			printf("Bust has been moved!\n");	
		}
 
		delay(200);
	}

	return 0 ;
}
