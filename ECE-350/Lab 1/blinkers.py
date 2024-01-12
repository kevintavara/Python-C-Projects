import RPi.GPIO as GPIO
import time

GPIO.setmode(GPIO.BCM)
GPIO.setup(18, GPIO.OUT) #setup pin 18
GPIO.setup(15, GPIO.OUT) #setup pin 15

while (True):
	GPIO.output(18, True)  #18 is true when 15 is false
	GPIO.output(15, False)
	time.sleep(0.5)        #delay between changes
	GPIO.output(15, True)  #15 is true when 18 is false
	GPIO.output(18, False)
	time.sleep(0.5)
