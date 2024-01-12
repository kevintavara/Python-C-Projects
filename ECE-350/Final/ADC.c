#include <wiringPi.h>
#include <softPwm.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#define ADC_CLK 28	//38
#define ADC_DIO 29	//40
#define ADC_CS  25	//37

typedef unsigned char uchar;

int main(void) {
	uchar adcVal;
	
	if(wiringPiSetup() == -1){
		printf("Setup for WiringPi Failed!");
		return 1;
	}

	// Setting up remainder inputs and outputs
	
	pinMode(ADC_CS, OUTPUT);
	pinMode(ADC_CLK, OUTPUT);
	
	while(1){
		// Generate ADC value based on potentiometer
		adcVal = ADC();
	}
}


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