/*
 *  TU/e Venus OGO
 *  Controller 
 *  Header
 */

#ifndef RobotController_h
#define RobotController_h

#include <Servo.h>

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

// Controller is a class that handles electronics of our venus robot
class RobotController {
  public:
    RobotController();
    void Accelerate();
    void Turn(double ang);          // Turning (left is negative, right positive)
    void Grab();                    // Grabbing
    void Scan();                    // Perform one sweep
    bool IsPerforming(action a);    // Check whether an action is being performed
  private:
    void addAction(action a);       // Set a state flag
    void removeAction(action a);    // Unset a state flag
    int state;                      // The current state of the robot, collection of actions defined above
    Servo wheelLeft;
    Servo wheelRight;
};

#endif
