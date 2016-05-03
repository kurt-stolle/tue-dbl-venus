#include "Arduino.h"
#include "Ultrasonic.h"

/*
 * Constructor
 */
Ultrasonic::Ultrasonic(pin trig) {
	this->triggerPin = trig;
	this->echoPin = trig;
}

Ultrasonic::Ultrasonic(pin trig, pin echo) {
	this->triggerPin = trig;
	this->echoPin = echo;
}

/* 
 * Public methods
 */

double Ultrasonic::Measure() {
	unsigned long duration;

	/* 
	 * Pinmode here for compatibility with US which uses
	 * the same pins for trig and echo...
	 */

	pinMode(this->triggerPin, OUTPUT);
	digitalWrite(this->triggerPin, LOW);
  
	delayMicroseconds(2);
	digitalWrite(this->triggerPin, HIGH);
 
	delayMicroseconds(10);
	digitalWrite(this->triggerPin, LOW);

	// Default pulseIn timeout is 1s, enough?
	pinMode(this->echoPin, INPUT);
	duration = pulseIn(this->echoPin, HIGH);

	return duration > 0 ? duration / 58.0 : -1;
}
