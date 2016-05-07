/*
 *  TU/e Venus OGO
 *  Ultrasonic
 *  Library for US sensors
 */

#ifndef Ultrasonic_h
#define Ultrasonic_h

#include "Layout.h"

class Ultrasonic {
public:
	void Attach(pin trig);
	void Attach(pin trig, pin echo);
	// Returns distance in centimeters, -1 if no object found.
	double GetDistance();
	void SendPulse();
private:
	pin triggerPin;
	pin echoPin;
	unsigned long echoStart = 0;
};

#endif
