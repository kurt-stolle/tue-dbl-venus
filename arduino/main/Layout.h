/*
 *  TU/e Venus OGO
 *  Layout
 *  Header for layout, to save memory
 */

#ifndef Layout_h
#define Layout_h

#include <ServoTimer2.h>

#define PIN_MOTOR_LEFT 12
#define PIN_MOTOR_RIGHT 13
#define PIN_SERVO_ULTRASOUND 11
#define PIN_TRIGGER_ULTRASOUND 9 
#define PIN_ECHO_ULTRASOUND 2 // Needs to be 2 or 3 for interrupts...

typedef unsigned char pin;
typedef ServoTimer2 Servo;

#endif
