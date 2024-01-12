#include <wiringPi.h>
#include <stdio.h>

#define R    8
#define B    10
#define G    11
#define Y    12
#define B_R  32
#define B_B  33
#define B_G  35
#define B_Y  37

int main(void) {
  	
	pinMode(R, OUTPUT); 
	pinMode(B_R, INPUT);
	pinMode(B, OUTPUT); 
	pinMode(B_B, INPUT);
	pinMode(G, OUTPUT); 
	pinMode(B_G, INPUT);
	pinMode(Y, OUTPUT); 
	pinMode(B_Y, INPUT);
  
  	int leds [4] = {8, 10, 11, 12};
  	int buttons [4] = {32, 33, 35, 37};
  	int count = 1;
  	int max = 10;
	int sequence [10];
  	int input [10];
  
    void generate_sequence() {
	randomSeed(millis()); //in this way is really random!!!

	for (int i = 0; i < count; i++) {
		sequence[i] = random(2,6);
	}
    }
  
    void show_sequence() {
	for (int i = 0; i < count; i++) {
		digitalWrite(leds[sequence[i]], HIGH);
		delay(500);
		digitalWrite(leds[sequence[i]], LOW);
		delay(500);
	}
   }
	
   void get_sequence() {
	int flag = 0; //this flag indicates if the sequence is correct
	for (int i = 0; i < count; i++) {
		flag = 0;
		while(flag == 0) {
			if (digitalRead(B_R) == LOW) {
				input[i] = 8;
				flag = 1;
				delay(200);
  				if (input[i] != leds[sequence[i]]) {
					printf("incorrect\n");
					return;
				}
			}
			if (digitalRead(B_B) == LOW) {
				input[i] = 10;
				flag = 1;
				delay(200);
				if (input[i] != leds[sequence[i]]) {
					printf("incorrect\n");
					return;
				}
			}
			if (digitalRead(B_G) == LOW) {
				input[i] = 11;
				flag = 1;
				delay(200);
				if (input[i] != leds[sequence[i]]) {
					printf("incorrect\n");
					return;
				}
			}
			if (digitalRead(B_Y) == LOW) {
				input[i] = 12;
				flag = 1;
				delay(200);
				if (input[i] != leds[sequence[i]]){
					printf("incorrect\n");
					return;
				}
			}
		}
	}
	printf("correct\n");
}