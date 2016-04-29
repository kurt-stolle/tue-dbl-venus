#include "Arduino.h"
#include "RobotController.h"
#include "Layout.h"


/*
 * Constructor
 */
RobotController::RobotController(){
  
}
/*
 * Public methods
 */
void RobotController::Accelerate() {
  this->wheelRight.attach(PIN_MOTOR_RIGHT);
  this->wheelLeft.attach(PIN_MOTOR_LEFT);
  
  this->wheelRight.writeMicroseconds(13000);
  this->wheelLeft.writeMicroseconds(13000);
}
 
void RobotController::Turn(double deg){
  
}

void RobotController::Grab(){
  
}

void RobotController::Scan(){
  
}

bool RobotController::IsPerforming(action a) {
  return (this->state & a);
}

/*
 * Private methods
 */
void RobotController::addAction(action a){
  this->state = this->state | a;
}

void RobotController::removeAction(action a){
  this->state = this->state ^ a;
}

