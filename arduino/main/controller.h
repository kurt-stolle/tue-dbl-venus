/*
 *  TU/e Venus OGO
 *  Controller 
 *  Header
 */

#ifndef Controller_h
#define Controller_h

// Arduino.h includes arduino functions we may want to use.
#include "Arduino.h"

// Typedefs used in the controller
typedef unsigned char pin;

// Enumerations for state
enum action {
  NONE = 0,
  MOVING_FORWARD = 1,
  MOVING_BACKWARD= 2,
  TURNING_LEFT = 4,
  TUNRING_RIGHT = 8,
  GRABBING = 16,
  SCANNING = 32
};

// ControllerLayout is a structure that tells us something about the configuration of the robot
struct ControllerLayout {
  pin wheelLeft;
  pin wheelRight;
  // etc...
};

// Controller is a class that handles electronics of our venus robot
class Controller {
  public:
    Controller(ControllerLayout* l);
    void Turn(double ang);          // Turning (left is negative, right positive)
    void Grab();                    // Grabbing
    void Scan();                    // Perform one sweep
    bool IsPerforming(action a);    // Check whether an action is being performed
  private:
    void addAction(action a);       // Set a state flag
    void removeAction(action a);    // Unset a state flag
    int state;                      // The current state of the robot, collection of actions defined above
    ControllerLayout* layout;       // The configuration of the robot
};

#endif
