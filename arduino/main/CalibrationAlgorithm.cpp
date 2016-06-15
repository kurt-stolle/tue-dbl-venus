#include "Algorithm.h"
#include "CalibrationAlgorithm.h"
#include "Arduino.h"
#include "RobotController.h"

void CalibrationAlgorithm::setup(RobotController* c) {
  this->setProcedure(Calibration::CALIBRATING); 
}

void CalibrationAlgorithm::loop(RobotController* c) {
  c->Forward(Speed::HALF);
  delay(2000);
  c->Turn(-90);
  while(c->IsPerforming(Action::TURNING)) continue;

  delay(10000);
  
  //c->ToggleUSTurn(true);
  //this->returnToLab(c);
}

