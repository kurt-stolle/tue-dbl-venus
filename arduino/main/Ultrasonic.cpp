#include "Arduino.h"
#include "Ultrasonic.h"

void Ultrasonic::Attach(pin trig) {
  this->triggerPin = trig;
  this->echoPin = trig;

  pinMode(this->triggerPin, OUTPUT);
}

void Ultrasonic::Attach(pin trig, pin echo) {
  this->triggerPin = trig;
  this->echoPin = echo;

  pinMode(this->triggerPin, OUTPUT);
  pinMode(this->echoPin,INPUT);
}

void Ultrasonic::SendPulse() {
	/* Pinmode here for compatibility with US which uses
	the same pins for trig and echo... */

	pinMode(this->triggerPin, OUTPUT);
  
	digitalWrite(this->triggerPin, LOW);

	delayMicroseconds(2);
	digitalWrite(this->triggerPin, HIGH);

	delayMicroseconds(5);
	digitalWrite(this->triggerPin, LOW);

  pinMode(this->triggerPin, INPUT);
}

double Ultrasonic::GetDistance() {
  double distance = -1;
  
	switch (digitalRead(this->echoPin)) {
	case HIGH:
    this->validResponse = true;
		this->echoStart = micros();
		break;
	case LOW:    
    if(this->validResponse) {
      distance = (micros() - this->echoStart) / 58.0;
    }

    this->validResponse = false;
    break;
	}

	return distance;
}
