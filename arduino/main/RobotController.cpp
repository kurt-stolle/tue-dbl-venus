#include "Arduino.h"
#include "RobotController.h"
#include "Layout.h"


/*
 * Constructor
 */
RobotController::RobotController() {
	this->wheelRight.attach(PIN_MOTOR_RIGHT);
	this->wheelLeft.attach(PIN_MOTOR_LEFT);
}

/*
 * Public methods
 */
void RobotController::Accelerate(int speed) {
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

	this->wheelLeft.write(90 - speed);
	this->wheelRight.write(90 + speed);
}

void RobotController::Reverse(int speed) {
	this->Accelerate(-speed);
}

void RobotController::Turn(double deg) {
	if (deg > 0.0)
		this->addAction(Action::TURNING_RIGHT);
	else
		this->addAction(Action::TURNING_LEFT);

	this->wheelLeft.write(90 - Speed::HALF);
	this->wheelRight.write(90 + Speed::HALF);
	delay(500); // Approximate value. To be determined..

	this->Accelerate(Speed::NONE);
}

void RobotController::Grab() {

}

void RobotController::Scan() {

}

bool RobotController::IsPerforming(Action a) {
	return (this->state & a);
}

/*
 * Private methods
 */
void RobotController::addAction(Action a) {
	this->state = this->state | a;
}

void RobotController::removeAction(Action a) {
	this->state = this->state ^ a;
}

