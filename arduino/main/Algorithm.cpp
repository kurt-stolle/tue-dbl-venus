#include "Algorithm.h"

/*
 * Implementation
 */
 #include "Algorithm.h"


template <class Procedure> void Algorithm<Procedure>::setProcedure(Procedure p) {
  this->procedure = p;
}

template <class Procedure> Procedure Algorithm<Procedure>::getProcedure() {
  return this->procedure;
}

template <class Procedure> bool Algorithm<Procedure>::avoid(RobotController* c) {
  bool left = (c->GetIRLeft() == Infrared::BLACK);
  short count = 0;
  double distance = 0.0;
  bool foundPassage = false;

  c->Reverse(Speed::FULL);
  delay(500);
  c->Forward(Speed::FULL);

  while (count < 3 && !foundPassage) {
    c->Turn(left ? -90 : 90);
    while (c->IsPerforming(Action::TURNING_LEFT) || c->IsPerforming(Action::TURNING_RIGHT));

    c->ResetTravelDist();
    delay(500);
    distance += c->GetTravelDist();

    c->Turn(left ? 90 : -90); // turn back to look at line/cliff
    while (c->IsPerforming(Action::TURNING_LEFT) || c->IsPerforming(Action::TURNING_RIGHT));

    unsigned long startDriveTime = millis();
    foundPassage = true;

    while ((startDriveTime - millis()) < 1000) {
      if (c->GetIRLeft() == Infrared::BLACK || c->GetIRRight() == Infrared::BLACK) {
        foundPassage = false;
        count++;
      }
    }
  }

  if (foundPassage) {
    c->ResetTravelDist();
    c->Turn(left ? 90 : -90);
    while (c->IsPerforming(Action::TURNING_LEFT) || c->IsPerforming(Action::TURNING_RIGHT));

    unsigned long startDriveTime = millis();
    while (c->GetTravelDist() < distance && (millis() - startDriveTime) < (10 * 1000));

    c->Turn(left ? -90 : 90);
    while (c->IsPerforming(Action::TURNING_LEFT) || c->IsPerforming(Action::TURNING_RIGHT));
  }

  return foundPassage;
}

/*
 * Define possiblle templates
 */

 #include "CalibrationAlgorithm.h"
 #include "ScoutAlgorithm.h"
 #include "CollectorAlgorithm.h"

 template class Algorithm<Scout::Procedure>;
 template class Algorithm<Calibration::Procedure>;
 template class Algorithm<Collector::Procedure>;
