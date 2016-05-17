#include "Arduino.h"
#include "RobotController.h"
#include "Layout.h"

// Convert degrees to milliseconds
double degToMs(double deg){
  // Thank you, Google.
  if (deg < -90) deg = 90;
  if (deg > 90) deg = 90;
  
  return map(deg, -90, 90, 1000, 2000);
}

// Constructor
RobotController::RobotController() {
	this->usSensorServo.attach(PIN_SERVO_ULTRASOUND);
	this->usSensorMain.Attach(PIN_TRIGGER_ULTRASOUND, PIN_ECHO_ULTRASOUND);

  this->lastMovementUpdate = micros();

  this->turnTarget = 0.0;
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
  if (this->turnTarget > 0.1){
    // Turn left
    this->removeAction(Action::TURNING_LEFT);
    this->addAction(Action::TURNING_RIGHT);

    modLeft = 1;
    modRight = 0;
  } else if (this->turnTarget < -0.1){
    // Turn right
    this->addAction(Action::TURNING_LEFT);
    this->removeAction(Action::TURNING_RIGHT);

    modLeft = 0;
    modRight = 1;
  } else {
    // Do not turn - no modulation needed
    this->removeAction(Action::TURNING_LEFT);
    this->removeAction(Action::TURNING_RIGHT);

    modLeft = 1;
    modRight = 1;
  }

  // Calculate the delta for how far we have turned (and moved) since last time
  double dSRight,dSLeft;
  {
    unsigned long T = micros();
    unsigned long dT = T - this->lastMovementUpdate; // Time delta in microseconds
    this->lastMovementUpdate = T;

    long double maxRotations = (WHEEL_RPM_FULL * dT / 600000.); // Rotations if speed would be maximum
    double maxSpeed = degToMs( this->IsPerforming(Action::MOVING_BACKWARD) ? Speed::FULL_REVERSE : Speed::FULL );
    
    dSRight = (2 * PI * WHEEL_RADIUS) * maxRotations * ( this->wheelRight.read() / maxSpeed ); // Distance deltas
    dSLeft  = (2 * PI * WHEEL_RADIUS) * maxRotations * (this->wheelLeft.read() / maxSpeed);
  }
  
  // When the delta of right is greater than the delta of left, then we must have been turning. We need to calculate the angle and remove it from the target.
  if (dSRight > dSLeft )  {
    turnTarget -= dSRight / WHEEL_DISTANCE_APART * (PI / 180.);
  } else if (dSLeft > dSRight) {
    turnTarget += dSLeft / WHEEL_DISTANCE_APART * (PI / 180.);
  }

  // Debug
  Serial.print("R: "); Serial.print(dSRight); Serial.print(" L: "); Serial.print(dSLeft); Serial.print("\n");


  // Apply our calculations
  double leftSpeed = speed * modLeft;
  double rightSpeed = speed * modRight;
  if (leftSpeed < 1){
    this->wheelLeft.detach();
  } else {
    if (!this->wheelLeft.attached()){
      this->wheelLeft.attach(PIN_MOTOR_LEFT);
    }

    
    this->wheelLeft.write(degToMs(-leftSpeed));
  }
  if (rightSpeed < 1 ){
    this->wheelRight.detach();
  } else {
    if (!this->wheelRight.attached()){
      this->wheelRight.attach(PIN_MOTOR_RIGHT);
    }
    
    this->wheelRight.write(degToMs(rightSpeed));
  }
}


