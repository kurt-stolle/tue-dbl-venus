#include "Arduino.h"
#include "RobotController.h"
#include "Layout.h"

/*
 * Unit conversion
 */
#define SERVO_MIN 750
#define SERVO_MAX 2250

inline double degToMs(double deg){
  // Thank you, Google.
  if (deg < -90) deg = -90;
  if (deg > 90) deg = 90;

  return map(deg, -90, 90, SERVO_MIN, SERVO_MAX);
}

inline double msToDeg(double ms){
  if (ms < SERVO_MIN) ms = SERVO_MIN;
  if (ms > SERVO_MAX) ms = SERVO_MAX;

  return map(ms, SERVO_MIN, SERVO_MAX, -90, 90);
}

/*
 * Constructor
 */
RobotController::RobotController() {
	this->usSensorServo.attach(PIN_SERVO_ULTRASOUND);
  this->usSensorServo.write(degToMs(0));
  this->lastUSTurn = millis();
	this->usSensorMain.Attach(PIN_TRIGGER_ULTRASOUND, PIN_ECHO_ULTRASOUND);
  this->usSensorAux.Attach(PIN_TRIGGER_ULTRASOUNDAUX, PIN_ECHO_ULTRASOUNDAUX);
  this->irSensorLeft.Attach(PIN_IR_LEFT, false);
  this->irSensorRight.Attach(PIN_IR_RIGHT, false);
  this->irLab.Attach(PIN_IR_LAB, true);
  this->irSample.Attach(PIN_IR_SAMPLE, true);
  this->servoGrabber.attach(PIN_SERVO_GRABBER);

  this->lastMovementUpdate = micros();
  this->distanceTraveled = 0.0;
  this->turnTarget = 0.0;
  this->state = Action::NONE;
  this->usDistance = DISTANCE_INFINITE;
  this->usDistanceAux = DISTANCE_INFINITE;
  this->usTurnEnabled = false;
  this->usAngle = 0.0;

  // Setup Xbee
  //this->xbee = new SoftwareSerial(2,3);
  //Serial.begin(9600); why here
  //this->xbee->begin(9600);
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

	this->movementSpeed = speed;
}

void RobotController::Reverse(int speed) {
	this->Forward(-speed);
}

// Turning
void RobotController::Turn(double deg) {
  if(deg > 0.0) {
    this->addAction(Action::TURNING_RIGHT);
    this->removeAction(Action::TURNING_LEFT);
  } else {
    this->addAction(Action::TURNING_LEFT);
    this->removeAction(Action::TURNING_RIGHT);
  }

  this->turnTarget = deg;
}

// Grab an object
void RobotController::Grab(bool grab) {
	this->servoGrabber.write(grab ? degToMs(90) : degToMs(0));
}

// Scan using the US sensor
void RobotController::Scan() {
  if(this->IsPerforming(Action::SCANNING)) {
    this->usDistance = DISTANCE_INFINITE;
  } else {
    this->addAction(Action::SCANNING);
  }

  if(this->IsPerforming(Action::SCANNINGAUX)) {
    this->usDistanceAux = DISTANCE_INFINITE;
  } else {
    this->addAction(Action::SCANNINGAUX);
  }

  this->usSensorMain.SendPulse();
	this->usSensorAux.SendPulse();
}

// UltraSonic listen
void RobotController::USListen() {
	double distance = this->usSensorMain.GetDistance();

	if (distance != -1) {
		this->usDistance = distance;
		this->removeAction(Action::SCANNING);
	}
}

void RobotController::USListenAux() {
  double distance = this->usSensorAux.GetDistance();

  if (distance != -1) {
    this->usDistanceAux = distance;
    this->removeAction(Action::SCANNINGAUX);
  }
}

double RobotController::GetUSAngle() {
   return msToDeg(this->usSensorServo.read());
}

void RobotController::SetUSAngle(double angle) {
  this->usAngle = angle;
}

double RobotController::GetUSDistance() {
	return this->usDistance;
}

double RobotController::GetUSDistanceAux() {
  return this->usDistanceAux;
}

void RobotController::ToggleUSTurn(bool enable) {
  this->usTurnEnabled = enable;
}

Infrared::Color RobotController::GetIRLeft() {
  return this->irSensorLeft.GetColor();  
}

Infrared::Color RobotController::GetIRRight() {
  return this->irSensorRight.GetColor();
}

Infrared::Color RobotController::GetIRSample() {
  return this->irSample.GetColor();
}

Infrared::Color RobotController::GetIRLab() {
  return this->irLab.GetColor();
}

// Comms
void RobotController::Communicate(){
  // Probably breaks bottom US sensor.
  
  /*if (Serial.available()) { // If data comes in from serial monitor, send it out to XBee
    this->xbee->write(Serial.read());
  }
  if (this->xbee->available()) { // If data comes in from XBee, send it out to serial monitor
    Serial.write(this->xbee->read());
  }*/
}

// State flag control
bool RobotController::IsPerforming(Action::Action a) {
	return (this->state & a);
}

void RobotController::addAction(Action::Action a) {
	this->state = this->state | a;
}

void RobotController::removeAction(Action::Action a) {
  this->state = this->state & (~a);
}

double RobotController::GetTravelDist() {
  return this->distanceTraveled;
}

void RobotController::ResetTravelDist() {
  this->distanceTraveled = 0.0;
}

// Update movement thread
void RobotController::UpdateMovement() {
  /*
   * Ultrasonic movement
   */
    
  if(!this->usTurnEnabled) {
    this->usSensorServo.write(degToMs(this->usAngle));
  } else if (millis() - this->lastUSTurn > CALIBRATION_TIME_US_TURN) {
    double newPosition = msToDeg(this->usSensorServo.read()) + 45.0;
    if (newPosition > 90.0){
      newPosition = -90.0;
      this->lastUSTurn = millis() + (CALIBRATION_TIME_US_TURN * 5); // allow some extra time to turn back
    } else {
      this->lastUSTurn = millis();
    }
    this->usSensorServo.write(degToMs(newPosition));
  }

  /*
   * Wheel movement
   */
  int speed;
  if (this->IsPerforming(Action::MOVING_FORWARD) || this->IsPerforming(Action::MOVING_BACKWARD)) {
    speed = this->movementSpeed;
  } else {
    speed = Speed::NONE;
  }

  // Calculate the delta for how far we have turned (and moved) since last time
  double dSRight,dSLeft;
  {
    unsigned long T = micros();
    unsigned long dT = T - this->lastMovementUpdate; // Time delta in microseconds
    this->lastMovementUpdate = T;

    long double maxRotations = (WHEEL_RPM_FULL * dT / 60000000.); // Rotations if speed would be maximum

    dSRight = (2 * PI * WHEEL_RADIUS) * maxRotations * ( (this->wheelRight.attached() ? -msToDeg(this->wheelRight.read()) : 0.00 ) / Speed::FULL ); // Distance deltas
    dSLeft  = (2 * PI * WHEEL_RADIUS) * maxRotations * ( (this->wheelLeft.attached() ? msToDeg(this->wheelLeft.read()) : 0.00 ) / Speed::FULL );
  }

  // When the delta of right is greater than the delta of left, then we must have been turning. We need to calculate the angle and remove it from the target.
  if (dSRight != dSLeft)  {
    this->turnTarget -= (dSLeft / (WHEEL_DISTANCE_APART / 2.0)) * (180.00 / PI);
  } else {
    this->distanceTraveled += dSLeft;
  }

  double modLeft;
  double modRight;

  // Apply a modulation to the wheel speed
  if (this->turnTarget > 5.0){
    // Turn left

    if(this->turnTarget > 15.0) {
      modLeft = 1;
      modRight = -1;
    } else {
      modLeft = 0.25;
      modRight = -0.25;
    }
  } else if (this->turnTarget < -5.0){
    // Turn right

    if(this->turnTarget < -15.0) {
      modLeft = -1;
      modRight = 1;
    } else {
      modLeft = -0.25;
      modRight = 0.25;
    }
  } else {
    // Do not turn - no modulation needed
    this->removeAction(Action::TURNING_LEFT);
    this->removeAction(Action::TURNING_RIGHT);

    this->turnTarget = 0;
    modLeft = 1;
    modRight = 1;
  }

  // Apply our calculations
  double leftSpeed = speed * modLeft;
  double rightSpeed = speed * modRight;

  if (abs(leftSpeed) < 1){
    this->wheelLeft.detach();
  } else {
    if (!this->wheelLeft.attached()){
      this->wheelLeft.attach(PIN_MOTOR_LEFT);
    }

    this->wheelLeft.write(degToMs(leftSpeed));
  }
  if (abs(rightSpeed) < 1){
    this->wheelRight.detach();
  } else {
    if (!this->wheelRight.attached()){
      this->wheelRight.attach(PIN_MOTOR_RIGHT);
    }

    this->wheelRight.write(degToMs(-rightSpeed));
  }
}
