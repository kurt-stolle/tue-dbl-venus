/*
 *  TU/e Venus OGO
 *  Layout
 *  Header for layout, to save memory
 */

#ifndef Layout_h
#define Layout_h

#include "ServoTimer2.h"

#define WHEEL_RPM_FULL 30 // RPM when on full power ahead, according to docs this value is between 50 and 60
#define WHEEL_RADIUS 2.75 // centimetres
#define WHEEL_DISTANCE_APART 9 // Distance the wheels are apart in centimetres

#define PIN_SERVO_GRABBER 10
#define PIN_MOTOR_LEFT 12
#define PIN_MOTOR_RIGHT 13
#define PIN_SERVO_ULTRASOUND 11
#define PIN_TRIGGER_ULTRASOUND 9 
#define PIN_ECHO_ULTRASOUND 2 // Needs to be 2 or 3 for interrupts

#define CALIBRATION_TIME_US_TURN 850 // Time in microseconds it takes for the 'head' to make a full turn to left or right at full turning speed

typedef unsigned char pin;
typedef ServoTimer2 Servo;

#endif
