/*
   Implementation
*/
#include "Algorithm.h"

template <class Procedure> void Algorithm<Procedure>::setProcedure(Procedure p) {
  this->procedure = p;
}

template <class Procedure> Procedure Algorithm<Procedure>::getProcedure() {
  return this->procedure;
}

template <class Procedure> bool Algorithm<Procedure>::avoid(RobotController* c) {
  bool left = !(c->GetIRLeft() == Infrared::BLACK);
  short count = 0;
  double distance = 0.0;
  bool foundPassage = false;

  while (count < 3 && !foundPassage) {
    c->Reverse(Speed::FULL);
    delay(500);
    c->Forward(Speed::FULL);
    
    c->Turn(left ? -90 : 90);
    while (c->IsPerforming(Action::TURNING_LEFT) || c->IsPerforming(Action::TURNING_RIGHT)) {
      if(c->GetIRLeft() == Infrared::BLACK || c->GetIRRight() == Infrared::BLACK) {
        return false;
      }
    }

    unsigned long startDriveTime = millis();
    
    c->ResetTravelDist();
    while((millis() - startDriveTime) < 500) {
      if(c->GetIRLeft() == Infrared::BLACK || c->GetIRRight() == Infrared::BLACK) {
        count = 10000; // can't go further to the left/right
        c->Reverse(Speed::FULL);
        delay(200);
        c->Forward(Speed::FULL);
        break;
      }
    }
    distance += c->GetTravelDist();

    c->Turn(left ? 90 : -90); // turn back to look at line/cliff
    while (c->IsPerforming(Action::TURNING_LEFT) || c->IsPerforming(Action::TURNING_RIGHT));

    foundPassage = true;
    startDriveTime = millis();

    while ((millis() - startDriveTime) < 1000) {
      if (c->GetIRLeft() == Infrared::BLACK || c->GetIRRight() == Infrared::BLACK) {
        foundPassage = false;
        count++;
        break;
      }
    }
  }

  if (foundPassage) {
    c->ResetTravelDist();
    c->Turn(left ? 90 : -90);
    while (c->IsPerforming(Action::TURNING_LEFT) || c->IsPerforming(Action::TURNING_RIGHT));

    unsigned long startDriveTime = millis();
    while (c->GetTravelDist() < distance && (millis() - startDriveTime) < (10 * 1000) && c->GetIRLeft() == Infrared::WHITE && c->GetIRRight() == Infrared::WHITE);

    c->Turn(left ? -90 : 90);
    while (c->IsPerforming(Action::TURNING_LEFT) || c->IsPerforming(Action::TURNING_RIGHT));
  }

  return foundPassage;
}

/*
   Define possiblle templates
*/
#if ALGORITHM == 1
#include "CalibrationAlgorithm.h"
template class Algorithm<Calibration::Procedure>;
#elif ALGORITHM == 2
#include "ScoutAlgorithm.h"
template class Algorithm<Scout::Procedure>;
#elif ALGORITHM == 3
#include "CollectorAlgorithm.h"
template class Algorithm<Collector::Procedure>;
#endif

