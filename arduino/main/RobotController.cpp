#include "Arduino.h"
#include "RobotController.h"
#include "Layout.h"

/*
 * Unit conversion
 */
#define SERVO_MIN 700
#define SERVO_MAX 2300
 
static double degToMs(double deg){
  // Thank you, Google.
  if (deg < -90) deg = 90;
  if (deg > 90) deg = 90;
  
  return map(deg, -90, 90, SERVO_MIN, SERVO_MAX);
}

static double msToDeg(double ms){
  if (ms < SERVO_MIN) ms = SERVO_MIN;
  if (ms > SERVO_MAX) ms = SERVO_MAX;

  return map(ms, SERVO_MIN, SERVO_MAX, -90, 90);
}

/*
 * Constructor
 */
RobotController::RobotController() {
	this->usSensorServo.attach(PIN_SERVO_ULTRASOUND);
  this->usSensorServo.write(degToMs(90));
  this->lastUSTurn = millis();
	this->usSensorMain.Attach(PIN_TRIGGER_ULTRASOUND, PIN_ECHO_ULTRASOUND);

  this->lastMovementUpdate = micros();

  this->turnTarget = 0.0;
}

/*
 * Helper methods
 */
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
  // disable interrupts. important code.
  noInterrupts();
  
  /*
   * Ultrasonic movement
   */
  if (millis() - this->lastUSTurn > CALIBRATION_TIME_US_TURN){
    this->usSensorServo.write(degToMs(-msToDeg(this->usSensorServo.read())));
    this->lastUSTurn = millis();
  }  

  /*
   * Wheel movement
   */
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
    this->turnTarget = 0;
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
    double maxSpeed = this->IsPerforming(Action::MOVING_BACKWARD) ? Speed::FULL_REVERSE : Speed::FULL;
    
    dSRight = (2 * PI * WHEEL_RADIUS) * maxRotations * ( (this->wheelRight.attached() ? msToDeg(this->wheelRight.read()) : 0.00 ) / maxSpeed ); // Distance deltas
    dSLeft  = (2 * PI * WHEEL_RADIUS) * maxRotations * ( (this->wheelLeft.attached() ? msToDeg(this->wheelLeft.read()) : 0.00 ) / maxSpeed);
  }

  
  Serial.print(dSRight); Serial.print(" : "); Serial.println(dSLeft);
  
  // When the delta of right is greater than the delta of left, then we must have been turning. We need to calculate the angle and remove it from the target.
  if (dSRight > dSLeft )  {
    this->turnTarget += (dSRight / WHEEL_DISTANCE_APART) * (PI / 180.00);
  } else if (dSLeft > dSRight) {
    this->turnTarget -= (dSLeft / WHEEL_DISTANCE_APART) * (PI / 180.00);
  }

  // Apply our calculations
  double leftSpeed = speed * modLeft;
  double rightSpeed = speed * modRight;
  if (leftSpeed < 1){
    this->wheelLeft.detach();
  } else {
    if (!this->wheelLeft.attached()){
      this->wheelLeft.attach(PIN_MOTOR_LEFT);
    }

    
    this->wheelLeft.write(degToMs(leftSpeed));
  }
  if (rightSpeed < 1 ){
    this->wheelRight.detach();
  } else {
    if (!this->wheelRight.attached()){
      this->wheelRight.attach(PIN_MOTOR_RIGHT);
    }
    
    this->wheelRight.write(degToMs(-rightSpeed));
  }

  // Allow interrupts again.
  interrupts();
}


