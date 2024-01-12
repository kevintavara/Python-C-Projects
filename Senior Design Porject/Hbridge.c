#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>

#define DIR			22
#define PWM			23	

void main() {
	int speed;
	
	if (wiringPiSetup () == -1)
		exit(1);
	
	pinMode(PWM, PWM_OUTPUT);
	pinMode(DIR, OUTPUT);

	while(1) {
		for(speed = 0; speed < 1024; ++speed) {
			digitalWrite(DIR, HIGH);
			pwmWrite(PWM, speed);
			delay(10);
		}
		delay(100);
		for(speed = 0; speed < 1024 ; ++speed) {
			digitalWrite(DIR, LOW);
			pwmWrite(PWM, speed);
			delay(10);
		}
	}
}

