#include "Algorithm.h"
#include "CalibrationAlgorithm.h"
#include "Arduino.h"
#include "RobotController.h"

void CalibrationAlgorithm::setup(RobotController* c) {
  this->setProcedure(Calibration::CALIBRATING); 
}

void CalibrationAlgorithm::loop(RobotController* c) {
  c->Forward(Speed::FULL);
  delay(1000);
  c->Turn(-90);
  while(c->IsPerforming(Action::TURNING)) delay(1);
  c->Forward(Speed::NONE);
  delay(1000);
}

