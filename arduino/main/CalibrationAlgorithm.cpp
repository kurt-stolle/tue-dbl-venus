#include "Algorithm.h"
#include "CalibrationAlgorithm.h"
#include "Arduino.h"
#include "RobotController.h"

void CalibrationAlgorithm::setup(RobotController* c) {
  this->setProcedure(Calibration::CALIBRATING);
}

void CalibrationAlgorithm::loop(RobotController* c) {
  //returnToLab(c);
  //c->ToggleUSTurn(true);

  c->Forward(Speed::FULL);

  while (true) {
    Serial.print("usMAIN: "); Serial.println(c->GetUSDistance());
    Serial.print("usAUX: "); Serial.println(c->GetUSDistanceAux());
    Serial.print("irLEFT: "); Serial.println(analogRead(PIN_IR_LEFT));
    Serial.print("irRIGHT: "); Serial.println(analogRead(PIN_IR_RIGHT));

    delay(300);
  }
}

