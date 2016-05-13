#include "Arduino.h"
#include "RobotController.h"
#include "Layout.h"

// Convert degrees to milliseconds
double degToMs(double deg){
  // Thank you, Google.
  if (deg < -90) deg = 90;
  if (deg > 90) deg = 90;
  
  return map(deg, -90, 90, 544, 2400);
}

// Constructor
RobotController::RobotController() {
	this->wheelRight.attach(PIN_MOTOR_RIGHT);
	this->wheelLeft.attach(PIN_MOTOR_LEFT);
	this->usSensorServo.attach(PIN_SERVO_ULTRASOUND);
	this->usSensorMain.Attach(PIN_TRIGGER_ULTRASOUND, PIN_ECHO_ULTRASOUND);

  this->lastMovementUpdate = micros();
}

// Move
void RobotController::Forward(int speed) {
	// Stop
	if (speed == Speed::NONE) {
		this->removeAction(Action::MOVING_BACKWARD);
		this->removeAction(Action::MOVING_FORWARD);
	}
	// Forward
	else if (speed > Speed::NONE) {
		this->removeAction(Action::MOVING_BACKWARD);
		this->addAction(Action::MOVING_FORWARD);
	}
	// Backward
	else {
		this->removeAction(Action::MOVING_FORWARD);
		this->addAction(Action::MOVING_BACKWARD);
	}

	this->movementSpeed = abs(speed);
}

void RobotController::Reverse(int speed) {
	this->Forward(-speed);
}

// Turning
void RobotController::Turn(double deg) {
  this->turnTarget = deg;
}

// Grab an object
void RobotController::Grab() {
	// TODO
}

// Scan using the IR sensor
void RobotController::Scan() {
  if(this->IsPerforming(Action::SCANNING)) {
    this->usDistance = DISTANCE_INFINITE;
  } else {
    this->addAction(Action::SCANNING);
  }
  
	this->usSensorMain.SendPulse();
}

// UltraSonic listen
void RobotController::USListen() {
	double distance = this->usSensorMain.GetDistance();

	if (distance != -1) {
		this->usDistance = distance;
		this->removeAction(Action::SCANNING);
	}
}

// Get the US distance
double RobotController::GetUSDistance() {
	return this->usDistance;
}

// State flag control
bool RobotController::IsPerforming(Action::Action a) {
	return (this->state & a);
}

void RobotController::addAction(Action::Action a) {
	this->state = this->state | a;
}

void RobotController::removeAction(Action::Action a) {
	this->state = this->state ^ a;
}

// Update movement thread
void RobotController::UpdateMovement() {
  int speed;
  if (this->IsPerforming(Action::MOVING_FORWARD)) {    
    speed = movementSpeed;
  } else if (this->IsPerforming(Action::MOVING_BACKWARD)) {
    speed = movementSpeed;
  } else {
    speed = Speed::NONE;
  }

  double modLeft;
  double modRight;

  // Apply a modulation to the wheel speed
  if (this->turnTarget > 0){
    // Turn left
    this->removeAction(Action::TURNING_LEFT);
    this->addAction(Action::TURNING_RIGHT);

    modLeft = 1;
    modRight = 0;
  } else if (this->turnTarget < 0){
    // Turn right
    this->addAction(Action::TURNING_LEFT);
    this->removeAction(Action::TURNING_RIGHT);

    modLeft = 0;
    modRight = 1;
  } else {
    // Do not turn - no modulation needed
    this->removeAction(Action::TURNING_LEFT);
    this->removeAction(Action::TURNING_RIGHT);
  }

  // Calculate the delta for how far we have turned (and moved) since last time
  double dSRight,dSLeft;
  {
    double dT = micros() - this->lastMovementUpdate; // Time delta in microseconds
    this->lastMovementUpdate = micros();
  
    double cExp = ((WHEEL_RPM_FULL * dT / 60000000.) / ( this->IsPerforming(Action::MOVING_BACKWARD) ? Speed::FULL_REVERSE : Speed::FULL)); // Intermediate value
    
    double dTurnRight = cExp * this->wheelRight.read(); // Turning deltas
    double dTurnLeft = 0 * this->wheelLeft.read();
  
    dSRight = (2 * PI * WHEEL_RADIUS) * dTurnRight; // Distance deltas
    dSLeft = (2 * PI * WHEEL_RADIUS) * dTurnLeft;
  }
  
  // When the delta of right is greater than the delta of left, then we must have been turning. We need to calculate the angle and remove it from the target.
  if (dSRight > dSLeft )  {
    turnTarget -= dSRight / WHEEL_DISTANCE_APART * (3.14159265 / 180.);
  } else if (dSLeft > dSRight) {
    turnTarget += dSLeft / WHEEL_DISTANCE_APART * (3.14159265 / 180.);
  }

  Serial.print("R: "); Serial.print(dSRight); Serial.print(" L: "); Serial.print(dSLeft); Serial.print("\n");


  // Apply our calculations
  this->wheelLeft.write(1500 + degToMs(speed * modLeft));
  this->wheelRight.write(1500 - degToMs(speed * modRight));
}


