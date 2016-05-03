/*
 *  TU/e Venus OGO
 *  Ultrasonic
 *  Header
 */

#ifndef Ultrasonic_h
#define Ultrasonic_h

#include "Layout.h"

class Ultrasonic {
public:
	Ultrasonic(pin trig);
	Ultrasonic(pin trig, pin echo);
	// Returns distance in centimeters, -1 if no object found.
	double doMeasure();
private:
	pin triggerPin;
	pin echoPin;
};

#endif
