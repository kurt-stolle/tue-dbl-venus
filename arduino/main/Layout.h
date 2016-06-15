/*
 *  TU/e Venus OGO
 *  Layout
 *  Header for layout, to save memory
 */

#ifndef Layout_h
#define Layout_h


 /*
  * Possible algorithms:
  * 1 - CalibrationAlgorithm
  * 2 - ScoutAlgorithm
  * 3 - CollectorAlgorithm
  */
#define ALGORITHM 1


#include "ServoTimer2.h"

#define WHEEL_RADIUS 3.3 // centimetres
#define WHEEL_DISTANCE_APART 11 // Distance the wheels are apart in centimetres

// Digital pins
#define PIN_SERVO_GRABBER 10
#define PIN_MOTOR_LEFT 12
#define PIN_MOTOR_RIGHT 13
#define PIN_SERVO_ULTRASOUND 11
#define PIN_TRIGGER_ULTRASOUND 9
#define PIN_ECHO_ULTRASOUND 2 // Needs to be 2 or 3 for interrupts
#define PIN_TRIGGER_ULTRASOUNDAUX 6
#define PIN_ECHO_ULTRASOUNDAUX 3 // Needs to be 2 or 3 for interrupts

#define PIN_LEFT_ENCODER 7
#define PIN_RIGHT_ENCODER 8

#define PIN_IR_LAB 5
#define PIN_IR_SAMPLE 4

// Analog pins
#define PIN_IR_LEFT 0
#define PIN_IR_RIGHT 1

#define CALIBRATION_WHEEL_HOLES 8
#define CALIBRATION_TIME_US_TURN 2000 // Time in microseconds it takes for the 'head' to make a full turn to left or right at full turning speed

typedef unsigned char pin;
typedef ServoTimer2 Servo;

#endif
