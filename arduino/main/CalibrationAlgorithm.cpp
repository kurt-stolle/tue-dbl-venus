#include "Algorithm.h"
#include "CalibrationAlgorithm.h"
#include "Arduino.h"
#include "RobotController.h"

void CalibrationAlgorithm::setup(RobotController* c) {
  this->setProcedure(Calibration::CALIBRATING); 
}

void CalibrationAlgorithm::loop(RobotController* c) {
  c->ToggleUSTurn(true);
  c->Forward(Speed::QUARTER);
  delay(3000);
  c->Turn(-90);
  while(c->IsPerforming(Action::TURNING)) continue;
}

