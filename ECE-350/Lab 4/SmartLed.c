#include <wiringPi.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>

#define R    	1
#define Button  4

int main(void) {

	// Global Variables

	bool status = false; 
	bool buttonflag = false;
	time_t ontime = time(NULL);
	time_t offtime = time(NULL);
	
	// Setup wiring pi
	
	if(wiringPiSetup() == -1) {
		printf("setup wiringPi failed!\n");
		return -1;
	}
	
	// Set input and outputs
	
	pinMode(1, OUTPUT);
	pinMode(4, INPUT);
	
	pullUpDnControl(4, PUD_UP);
	
	while(1) {
		
		// Reset current status in newStatus
		
		bool newStatus = status;
		
		// Button has not been pressed yet
		
		if (buttonflag == false) {
		
			// Check if button is pressed
		
			if (digitalRead(Button) == LOW) {
				delay(50);
				
				// If pressed negate status and set flag to true
				
				buttonflag = true;
				newStatus = !newStatus;
			}
		}
		
		// Once button has been pressed once
		
		else {
		
			// Check if button is no longer pressed
		
			if (digitalRead(Button) == HIGH) {
				buttonflag = false;
			}
		}	
		
		// These if statements simply turn led off or on depending on status
		
		if (newStatus == true){
			digitalWrite(R, HIGH);	
		}
		
		if (newStatus == false){
			digitalWrite(R, LOW);	
		}
		
		// If status does not match then we know a change has occured
		
		if (newStatus != status) {
		
			// Since change occured print current time
		
			struct tm* ptr;
			time_t mytime = time(NULL);
			ptr = localtime(&mytime);
			printf("%s", asctime(ptr));
			
			// Depending on whether led was off or on, print the duration of status
			
			if (newStatus == true) {
				ontime = time(NULL);
				printf("Led off for %f\n", difftime(ontime,offtime));
			}
			else {
				offtime = time(NULL);
				printf("Led on for %f\n", difftime(offtime,ontime));
			}
		}
		status = newStatus;
	}
}
	
