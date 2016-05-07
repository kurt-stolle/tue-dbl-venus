#include "Arduino.h"
#include "Ultrasonic.h"

void Ultrasonic::Attach(pin trig) {
  this->triggerPin = trig;
  this->echoPin = trig;
}

void Ultrasonic::Attach(pin trig, pin echo) {
  this->triggerPin = trig;
  this->echoPin = echo;
}

void Ultrasonic::SendPulse() {
	/* Pinmode here for compatibility with US which uses
	the same pins for trig and echo... */

	pinMode(this->triggerPin, OUTPUT);
	digitalWrite(this->triggerPin, LOW);

	delayMicroseconds(2);
	digitalWrite(this->triggerPin, HIGH);

	delayMicroseconds(10);
	digitalWrite(this->triggerPin, LOW);
}

double Ultrasonic::GetDistance() {
	switch (digitalRead(this->echoPin)) {
	case HIGH:
		this->echoStart = micros();
		break;
	case LOW:
		return (micros() - echoStart) / 58.0;
	}

	return -1;
}