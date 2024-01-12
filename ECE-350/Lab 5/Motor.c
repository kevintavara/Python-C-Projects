#include <wiringPi.h>
#include <softPwm.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

// Defining all pins

#define ADC_CLK 28
#define ADC_DIO 29
#define ADC_CS  25
#define BUTTON  5
#define IA  26
#define IB  23
#define LED1 27
#define LED2 0
#define LED3 1
#define LED4 2
#define LED5 3
#define LED6 4
#define LED7 6
#define LED8 21
#define LED9 22
#define LED10 24

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
	pinMode(LED1, OUTPUT);
	pinMode(LED2, OUTPUT);
	pinMode(LED3, OUTPUT);
	pinMode(LED4, OUTPUT);
	pinMode(LED5, OUTPUT);
	pinMode(LED6, OUTPUT);
	pinMode(LED7, OUTPUT);
	pinMode(LED8, OUTPUT);
	pinMode(LED9, OUTPUT);
	pinMode(LED10, OUTPUT);
	
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
		// Multiple if statements used to determine how many leds should be on
		// ADC range from 0-255 so 25 intervals were chosen
		if (adcVal > 25){
			digitalWrite(LED1, HIGH);
		}
		else {
			digitalWrite(LED1, LOW);
		}
		if (adcVal > 50){
			digitalWrite(LED2, HIGH);
		}
		else {
			digitalWrite(LED2, LOW);
		}
		if (adcVal > 75){
			digitalWrite(LED3, HIGH);
		}
		else {
			digitalWrite(LED3, LOW);
		}
		if (adcVal > 100){
			digitalWrite(LED4, HIGH);
		}
		else {
			digitalWrite(LED4, LOW);
		}
		if (adcVal > 125){
			digitalWrite(LED5, HIGH);
		}
		else {
			digitalWrite(LED5, LOW);
		}
		if (adcVal > 150){
			digitalWrite(LED6, HIGH);
		}
		else {
			digitalWrite(LED6, LOW);
		}
		if (adcVal > 175){
			digitalWrite(LED7, HIGH);
		}
		else {
			digitalWrite(LED7, LOW);
		}
		if (adcVal > 200){
			digitalWrite(LED8, HIGH);
		}
		else {
			digitalWrite(LED8, LOW);
		}
		if (adcVal > 225){
			digitalWrite(LED9, HIGH);
		}
		else {
			digitalWrite(LED9, LOW);
		}
		if (adcVal > 250){
			digitalWrite(LED10, HIGH);
		}
		else {
			digitalWrite(LED10, LOW);
		}
	}
	
	return 0;
}
		
		
		
		
		
		
		
		
		
		
		
