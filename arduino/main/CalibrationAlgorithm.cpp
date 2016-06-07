#include "Algorithm.h"
#include "CalibrationAlgorithm.h"
#include "Arduino.h"
#include "RobotController.h"

void CalibrationAlgorithm::setup(RobotController* c) {
  this->setProcedure(Calibration::CALIBRATING);
  c->Forward(Speed::FULL);
  
}

void CalibrationAlgorithm::loop(RobotController* c) {
  // lol nope
}

