#include <wiringPi.h>
#include <softPwm.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

// Defining all pins

#define ADC_CLK 28	//38
#define ADC_DIO 29	//40
#define ADC_CS  25	//37

#define BUTTON  5

#define IA  26
#define IB  23

typedef unsigned char uchar;

uchar ADC(void) {
	
	uchar i;
	uchar data1, data2;
	
	// Used to set up ADC
	
	digitalWrite(ADC_CS, 0);
	digitalWrite(ADC_CLK, 0);
	digitalWrite(ADC_DIO, 1);
	delay(0.002);
	digitalWrite(ADC_CLK, 1);
    delay(0.002);
    
    digitalWrite(ADC_CLK, 0);
    digitalWrite(ADC_DIO, 1);
    delay(0.002);
    digitalWrite(ADC_CLK, 1);
    delay(0.002);
    
    digitalWrite(ADC_CLK, 0);
    digitalWrite(ADC_DIO, 0);
    delay(0.002);
    digitalWrite(ADC_CLK, 1);
    digitalWrite(ADC_DIO, 1);
    delay(0.002);
    digitalWrite(ADC_CLK, 0);
    digitalWrite(ADC_DIO, 1);
    delay(0.002);
    
    // For loop used to generate first data
    
    data1 = 0;
    for(i=0;i<8;i++) {
		digitalWrite(ADC_CLK, 1);
		delay(0.002);
		digitalWrite(ADC_CLK, 0);
		delay(0.002);
		pinMode(ADC_DIO, INPUT);
		data1 = data1 << 1 | digitalRead(ADC_DIO);
	}
	
	// For loop used to generate second data
	
	data2 = 0;
    for(i=0;i<8;i++) {
		data2 = data2 | digitalRead(ADC_DIO) << i;
		digitalWrite(ADC_CLK, 1);
		delay(0.002);
		digitalWrite(ADC_CLK, 0);
		delay(0.002);
	}
	
	digitalWrite(ADC_CS, 1);
	
	// Changed to output since input is needed only for loop
	pinMode(ADC_DIO, OUTPUT);
	
	// Check is data matches if so return first data cause accurate
	
	if (data1 == data2) {
		return data1;
	}
	else {
		// Error occured so check if data1 is greater if so data2 is accurate
		if (data1 > data2) {
			return data2;
		}
		return 0;
	}
}

int main(void) {
	uchar adcVal;
	bool flag, direction;
	
	if(wiringPiSetup() == -1){
		printf("Setup for WiringPi Failed!");
		return 1;
	}
	
	// Setting up remainder inputs and outputs
	
	pinMode(ADC_CS, OUTPUT);
	pinMode(ADC_CLK, OUTPUT);
	pinMode(IA, OUTPUT);
	pinMode(IB, OUTPUT);
	pinMode(BUTTON, INPUT);
	
	pullUpDnControl(BUTTON, PUD_UP); 
	
	// Create two PWM for IA and IB since they control motor
	
	softPwmCreate(IA, 0, 255);
	softPwmCreate(IB, 0, 255);

	while(1){
		// Check if button pressed
		if (flag == false){
			if (digitalRead(BUTTON) == LOW){
				delay(50);
				// When pressed flag changes to true and direction inverted
				flag = true;
				direction = !direction;
			}
		}
		else {
			if (digitalRead(BUTTON) == HIGH){
				// Reset flag when not pressed
				flag = false;
			}
		}
		// Generate ADC value based on potentiometer
		adcVal = ADC();
		// adcVal determines speed of motor
		// CW
		if (direction == true){
			digitalWrite(IA, LOW);
			softPwmWrite(IB, adcVal);
		}
		// CCW
		else {
			digitalWrite(IB, LOW);
			softPwmWrite(IA, adcVal);
		}
	}
	
	return 0;
}
		
		
		
		
		
		
		
		
		
		
		
