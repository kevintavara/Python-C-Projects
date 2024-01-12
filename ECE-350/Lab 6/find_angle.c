#include <stdio.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <stdlib.h>
#include <lcd.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>

#define PI 3.141592
#define X_REG 0x32
#define Y_REG 0x34
#define Z_REG 0x36
#define BUTTON 25

short int axis_sample_average(int axis, int df);
short int axis_sample(int axis,int df);

int main(void)
{
        int df = 0;
        int setup, x, y;
        double x1;
        bool rod;
        char lcd[100];
        short int data = 0;
        short int data2 = 0;
        int datasimple = 0;
        double accelX, accelY, accelZ;

	if(wiringPiSetup() == -1){
		printf("wiringPi setup failed !\n");
		exit(1);
	}
	setup = lcdInit(2, 16, 8, 5, 21, 22, 6, 23, 24, 26, 27, 28, 29);
        
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
                
	pinMode(BUTTON, INPUT);
	pullUpDnControl(BUTTON, PUD_UP);
        
        while(1){
                accelX = axis_sample_average(X_REG,df);
                accelY = axis_sample_average(Y_REG,df);
                accelZ = axis_sample_average(Z_REG,df);
                y = 0;
                
                // Check if button pressed
                if (digitalRead(BUTTON) == LOW){
                        delay(50);
                        // When pressed rod is inverted
                        rod = !rod;
                }
                // Degrees
                x1 = atan(accelX/sqrt(accelY*accelY+accelZ*accelZ));
                x1 *= 180;
                x1 /= PI;
                x = round(x1);
                // Check what qudarant angle is in
		if (accelY < 0){
                	x = 180 - x;
                }
                if (accelX < 0 && accelY < 0){
                        x = 360 - x;
                        y = -1;
                }
                // Check if negative
                if (x < 0){
                        x = x * -1;
                        y = -1;
                }
                // Radians
                if (rod == true){
                        x1 = x * PI/180;
                }
                // Degrees
		if (rod == false){
                        // Check if negative or postive with y
                	if (y < 0){
                		if (x > 99){
                			sprintf(lcd, "- %d deg", x);
                		}
                		else if (x > 9){
                			sprintf(lcd, "- 0%d deg", x);
                		}
                		else {
                			sprintf(lcd, "- 00%d deg", x);
                		}
                	}
                	else{
                		if (x > 99){
                			sprintf(lcd, "+ %d deg", x);
                		}
                		else if (x > 9){
                			sprintf(lcd, "+ 0%d deg", x);
                		}
                		else {
                			sprintf(lcd, "+ 00%d deg", x);
                		}
                	}
                }
                // Radians
                else{
                        // Check if postive or negative with y
                	if (y < 0){
                                sprintf(lcd, "- %.2f rad", x1);
                	}
                	else{
                                sprintf(lcd, "+ %.2f rad", x1);
                	}
                }
                // Reset y
		y = 0;
		
                // Clean up LCD
		lcdClear(setup);
		lcdPrintf(setup, lcd);
		
                // Used for testing
                fprintf(stderr,"x:%f\n",accelX); // X
                fprintf(stderr,"y:%f\n",accelY); // Y
                fprintf(stderr,"z:%f\n\n",accelZ); // Z
                
                // Delay for LCD
                delay(1000);
        }

        return 0;
}

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
