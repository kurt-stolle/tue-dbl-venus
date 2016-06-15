#include "Algorithm.h"
#include "CalibrationAlgorithm.h"
#include "Arduino.h"
#include "RobotController.h"

void CalibrationAlgorithm::setup(RobotController* c) {
  this->setProcedure(Calibration::CALIBRATING); 
}

void CalibrationAlgorithm::loop(RobotController* c) {
  this->returnToLab(c);
}

