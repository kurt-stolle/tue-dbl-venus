/*
 *  TU/e Venus OGO
 *  Controller
 *  Acts as interface to the robot hardware
 */

#ifndef RobotController_h
#define RobotController_h

#include "ServoTimer2.h"
#include "Ultrasonic.h"

#define DISTANCE_INFINITE 10000

 // Enumerations for state
namespace Action {
  enum Action {
  	NONE = 0,
  	MOVING_FORWARD = 1,
  	MOVING_BACKWARD = 2,
  	TURNING_LEFT = 4,
  	TURNING_RIGHT = 8,
  	GRABBING = 16,
  	SCANNING = 32
  };
};

// Enumerations for speed
namespace Speed {
	enum Speed {
		NONE = 0,
		HALF = 45,
		FULL = 90,
    HALF_REVERSE = -45,
    FULL_REVERSE = -90
	};
};

class RobotController {
public:
	RobotController();

	void Forward(int speed);
  void Reverse(int speed);
	void Turn(double deg);									// Turning (left is negative, right positive)
	void UpdateMovement();

  void Grab(bool grab);										// Grabbing
	void Scan();                            // Perform one sweep
	void USListen();												// Listens for response to Scan()
	double GetUSDistance();									// Retrieves main US sensor data

	bool IsPerforming(Action::Action a);    // Check whether an action is being performed
private:
	void addAction(Action::Action a);       // Set a state flag
	void removeAction(Action::Action a);    // Unset a state flag
 
	volatile int state;                     // The current state of the robot, collection of actions defined above
  unsigned long lastMovementUpdate;
  unsigned long lastUSTurn;

	// Servo wheels
	Servo wheelLeft;
	Servo wheelRight;
  Servo servoGrabber;
	
  // The current movement speed
	int movementSpeed;

  //  Ultrasonic sensor
	Servo usSensorServo;
	Ultrasonic usSensorMain;
  volatile double usDistance;

  // Target
  volatile double turnTarget; // The turning target, set by the Turn function and used to control how far we are turning 
};

#endif
