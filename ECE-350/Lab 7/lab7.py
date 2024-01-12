#! /usr/bin/python

# loads the necessary header files to enable the PiCamera, OpenCv, and their dependencies

import RPi.GPIO as GPIO
import time
from picamera import PiCamera
from time import sleep
import numpy as np
import cv2

# Adeept Lesson 14 ultrasonic lab code
# Checdist stores the distance between the chair from the HC-SR04 in feet

def checkdist():
	GPIO.output(16, GPIO.HIGH)
	time.sleep(0.000015)
	GPIO.output(16, GPIO.LOW)
	while not GPIO.input(18):
		pass
	t1 = time.time()
	while GPIO.input(18):
		pass
	t2 = time.time()
	return ((t2-t1)*340/2) * 3.28 # converts meters into feet

GPIO.setmode(GPIO.BOARD) # sets GPIO to physical pin position
GPIO.setup(16,GPIO.OUT,initial=GPIO.LOW)
GPIO.setup(18,GPIO.IN)
time.sleep(2)

while True:
	distance_from_chair = checkdist() # stores the distance 
	if distance_from_chair <= 1.5: #checks if the distance is less than one and half feet. This indicates the seat is occupied 
		camera = PiCamera()
		time.sleep(5)
		camera.start_preview()
		camera.capture('/home/pi/Documents/Lab7/image.jpg') # captures the image of the person sitting in the chair
		camera.close()
		# Begins the facial recognition process and identfies the person's face sitting in the chair
		# Uses the predefined classifiers that have already been trained for basic facial detection (Haar Cascade)
		face_cascade = cv2.CascadeClassifier('/usr/local/lib/python3.7/dist-packages/cv2/data/haarcascade_frontalface_default.xml')
		eye_cascade = cv2.CascadeClassifier('/usr/local/lib/python3.7/dist-packages/cv2/data/haarcascade_eye.xml')

		img = cv2.imread('/home/pi/Documents/Lab7/image.jpg')
		gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)

		faces = face_cascade.detectMultiScale(gray, 1.3, 5)
		for (x,y,w,h) in faces: # for loop places a blue rectangle around the face and a green recntangle around the two eyes
			img = cv2.rectangle(img,(x,y),(x+w,y+h),(255,0,0),2) # creates the rectangle around the person's face
			roi_gray = gray[y:y+h, x:x+w]
			roi_color = img[y:y+h, x:x+w]
			eyes = eye_cascade.detectMultiScale(roi_gray)
			for (ex, ey, ew, eh) in eyes: # creates the green rectangle around the person's eyes
				cv2.rectangle(roi_color,(ex,ey),(ex+ew,ey+eh),(0,255,0),2)

		cv2.imshow('img',img) # shows the image captured with the blue and green rectangles
		cv2.waitKey(0)
		cv2.destroyAllWindows()
			

# except KeyboardInterrupt:
GPIO.cleanup()