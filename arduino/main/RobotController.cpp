#include "Arduino.h"
#include "RobotController.h"
#include "Layout.h"

RobotController::RobotController() {
	this->wheelRight.attach(PIN_MOTOR_RIGHT);
	this->wheelLeft.attach(PIN_MOTOR_LEFT);
	this->usSensorServo.attach(PIN_SERVO_ULTRASOUND);
	this->usSensorMain.Attach(PIN_TRIGGER_ULTRASOUND, PIN_ECHO_ULTRASOUND);
}

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

void RobotController::Turn(double deg) {
	if (deg > 0.0){
		this->addAction(Action::TURNING_RIGHT);
	} else if(deg == 0.0) {
	  this->removeAction(Action::TURNING_RIGHT);
    this->removeAction(Action::TURNING_LEFT);
	} else {
		this->addAction(Action::TURNING_LEFT);
	}
}

void RobotController::UpdateMovement() {  
	if (this->IsPerforming(Action::MOVING_FORWARD)) {    
		if (this->IsPerforming(Action::TURNING_LEFT)) {
			this->wheelLeftSpeed = movementSpeed >> 1; // Divide by 2 ;)
			this->wheelRightSpeed = movementSpeed;

		} else if (this->IsPerforming(Action::TURNING_RIGHT)) {
			this->wheelLeftSpeed = movementSpeed;
			this->wheelRightSpeed = movementSpeed >> 1;
		}
		else {
			this->wheelLeftSpeed = movementSpeed;
			this->wheelRightSpeed = movementSpeed;
		}
	}
	else if (this->IsPerforming(Action::MOVING_BACKWARD)) {
		if (this->IsPerforming(Action::TURNING_LEFT)) {
			this->wheelLeftSpeed = -movementSpeed;
			this->wheelRightSpeed = -(movementSpeed >> 1);
		}
		else if (this->IsPerforming(Action::TURNING_RIGHT)) {
			this->wheelLeftSpeed = -(movementSpeed >> 1);
			this->wheelRightSpeed = -movementSpeed;
		}
		else {
			this->wheelLeftSpeed = -movementSpeed;
			this->wheelRightSpeed = -movementSpeed;
		}
	}
	else {
		if (this->IsPerforming(Action::TURNING_LEFT)) {
			this->wheelLeftSpeed = -Speed::HALF;
			this->wheelRightSpeed = Speed::HALF;
		}
		else if (this->IsPerforming(Action::TURNING_RIGHT)) {
			this->wheelLeftSpeed = Speed::HALF;
			this->wheelRightSpeed = -Speed::HALF;
		}
		else {
			this->wheelLeftSpeed = Speed::NONE;
			this->wheelRightSpeed = Speed::NONE;
		}
	}

	this->wheelLeft.write(1500 + this->wheelLeftSpeed);
	this->wheelRight.write(1500 - this->wheelRightSpeed);
}

void RobotController::Grab() {
	// TODO
}

void RobotController::Scan() {
  if(this->IsPerforming(Action::SCANNING)) {
    this->usDistance = DISTANCE_INFINITE;
  } else {
    this->addAction(Action::SCANNING);
  }
  
	this->usSensorMain.SendPulse();
}

void RobotController::USListen() {
	double distance = this->usSensorMain.GetDistance();

	if (distance != -1) {
		this->usDistance = distance;
		this->removeAction(Action::SCANNING);
	}
}

double RobotController::getUSDistance() {
	return this->usDistance;
}

/*
 * State control
 */

bool RobotController::IsPerforming(Action::Action a) {
	return (this->state & a);
}

void RobotController::addAction(Action::Action a) {
	this->state = this->state | a;
}

void RobotController::removeAction(Action::Action a) {
	this->state = this->state ^ a;
}

