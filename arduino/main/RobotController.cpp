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
  int mot1 = 1500;
  int mot2 = 1495;

  
  this->wheelRight.attach(PIN_MOTOR_RIGHT);
  this->wheelLeft.attach(PIN_MOTOR_LEFT);

  this->wheelRight.writeMicroseconds(mot1);
  this->wheelLeft.writeMicroseconds(mot2);

  /*for(int i = 0; i < 20; i++) {
    this->wheelRight.writeMicroseconds(mot1);
    this->wheelLeft.writeMicroseconds(mot2);

    Serial.print("mot1: ");
    Serial.println(mot1);
    Serial.print("mot2: ");
    Serial.println(mot2);
    
    mot1 += 5;
    mot2 -= 5;

    delay(200);
  }*/
  
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

