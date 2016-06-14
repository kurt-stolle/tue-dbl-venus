/*
 *  TU/e Venus OGO
 *  Controller
 *  Acts as interface to the robot hardware
 */

#ifndef RobotController_h
#define RobotController_h

#include "ServoTimer2.h"
#include "Ultrasonic.h"
#include "Infrared.h"
//#include "SoftwareSerial.h"

#define DISTANCE_INFINITE 10000

#define WHEEL_AVERAGE 8 // # samples running averager

 // Enumerations for state
namespace Action {
  enum Action {
  	NONE = 0,
  	MOVING_FORWARD = 1,
  	MOVING_BACKWARD = 2,
  	TURNING_LEFT = 4,
  	TURNING_RIGHT = 8,
  	GRABBING = 16,
  	SCANNING = 32,
    SCANNINGAUX = 64,
    TURNING = 128
  };
};

// Enumerations for speed
namespace Speed {
	enum Speed {
		NONE = 0,
    QUARTER = 20,
		HALF = 45,
		FULL = 90,
    QUARTER_REVERSE = -20,
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
  double GetTravelDist();
  void ResetTravelDist();

  void Grab(bool grab);										// Grabbing
	void Scan();                            // Send one US pulse
	void USListen();												// Listens for response to Scan()
  double GetUSAngle();                    // Get the current angle of the main US sensor
  void SetUSAngle(double angle);
  void USListenAux();
	double GetUSDistance();									// Retrieves main US sensor data
  double GetUSDistanceAux();
  void ToggleUSTurn(bool enable);

  Infrared::Color GetIRLeft();
  Infrared::Color GetIRRight();
  Infrared::Color GetIRSample();
  Infrared::Color GetIRLab();

	bool IsPerforming(Action::Action a);    // Check whether an action is being performed
private:
  // RPM Calculations
  double RPM;
  double RPMRunningAverage[WHEEL_AVERAGE] = {0.0};
  uint8_t RPMLastEncoderValue;
  unsigned long RPMLastEncoderEdge; // in milliseconds

  // Actions
	void addAction(Action::Action a);       // Set a state flag
	void removeAction(Action::Action a);    // Unset a state flag

	volatile int state;                     // The current state of the robot, collection of actions defined above
  unsigned long lastMovementUpdate;
  unsigned long lastUSTurn;
  volatile bool usTurnEnabled;
  volatile double usAngle;

	// Servo wheels
	Servo wheelLeft;
	Servo wheelRight;
  Servo servoGrabber;

  // The current movement speed
	int movementSpeed;

  //  Ultrasonic sensor
	Servo usSensorServo;
	Ultrasonic usSensorMain;
  Ultrasonic usSensorAux;
  Infrared irSensorLeft;
  Infrared irSensorRight;
  Infrared irLab;
  Infrared irSample;
  volatile double usDistance;
  volatile double usDistanceAux;

  // Target
  volatile double turnTarget; // The turning target, set by the Turn function and used to control how far we are turning
  volatile double distanceTraveled;       // Keeps track of the distance traveled in centimeters.
};

#endif
