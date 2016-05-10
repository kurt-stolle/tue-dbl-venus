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

  noInterrupts();

  this->validResponse = false;

	pinMode(this->triggerPin, OUTPUT);
	digitalWrite(this->triggerPin, LOW);

	delayMicroseconds(2);
	digitalWrite(this->triggerPin, HIGH);

	delayMicroseconds(5);
	digitalWrite(this->triggerPin, LOW);

  pinMode(this->triggerPin, INPUT);

  this->validResponse = true;

  interrupts();
}

double Ultrasonic::GetDistance() {
  if (!this->validResponse) return -1.0;
  
  Serial.print("US pin: ");
  Serial.println(digitalRead(this->echoPin) ? "HIGH" : "LOW");
  
	switch (digitalRead(this->echoPin)) {
	case HIGH:
		this->echoStart = micros();
		break;
	case LOW:
		return (micros() - this->echoStart) / 58.0;
	}

	return -1;
}
