#include "ScoutAlgorithm.h"
#include "Arduino.h"
#include "RobotController.h"

void ScoutAlgorithm::setup(RobotController* c){

}

bool ScoutAlgorithm::avoid(RobotController* c) {
  bool left = (c->GetIRLeft() == Infrared::BLACK);
  short count = 0;
  double distance = 0.0;

  c->Reverse(Speed::FULL);
  delay(500);
  c->Forward(Speed::FULL);

  while(count < 3 && !foundPassage) {
    c->Turn(left ? -90 : 90);
    while(c->IsPerforming(Action::TURNING_LEFT) || c->IsPerforming(Action::TURNING_RIGHT));

    c->ResetTravelDist();
    delay(500);
    distance += c->GetTravelDist();

    c->Turn(left ? 90 : -90); // turn back to look at line/cliff
    while(c->IsPerforming(Action::TURNING_LEFT) || c->IsPerforming(Action::TURNING_RIGHT));

    unsigned long startDriveTime = millis();

    while((startDriveTime - millis()) < 1000) {
      if(c->GetIRLeft() == Infrared::BLACK || c->GetIRRight() == Infrared::BLACK) {
        if count < 3 {
          continue;
        }

        return false
      }
    }

    break;
  }

  c->ResetTravelDist();
  c->Turn(left ? 90 : -90);
  while(c->IsPerforming(Action::TURNING_LEFT) || c->IsPerforming(Action::TURNING_RIGHT));

  unsigned long startDriveTime = millis();
  while(c->GetTravelDist() < distance && (millis() - startDriveTime) < (10 * 1000));

  c->Turn(left ? -90 : 90);
  while(c->IsPerforming(Action::TURNING_LEFT) || c->IsPerforming(Action::TURNING_RIGHT));

  return true
}

void ScoutAlgorithm::loop(RobotController* c){

  if(this->getProcedure() == Scout::SWEEP) {
    c->ToggleUSTurn(false);
    c->SetUSAngle(0);
    c->Forward(Speed::HALF);
    c->Turn(-90);
    while(c->IsPerforming(Action::TURNING_LEFT)) {
      if((c->GetUSDistance() - c->GetUSDistanceAux()) > 10.0 && c->GetUSDistance() < 250.0) { // 3m for inacurracy protection
        c->Turn(0);
        c->Forward(Speed::NONE);
        this->setProcedure(Scout::FINDING_SAMPLE);
        return;
      }
    }

    c->Turn(180);
    while(c->IsPerforming(Action::TURNING_RIGHT)) {
      if((c->GetUSDistance() - c->GetUSDistanceAux()) > 10.0 && c->GetUSDistance() < 250.0) {
        c->Turn(0);
        c->Forward(Speed::NONE);
        this->getProcedure() = Scout::FINDING_SAMPLE;
        return;
      }
    }

    // Try to find mountain
    c->Turn(-180);
    while(c->IsPerforming(Action::TURNING_LEFT)) {
      if((c->GetUSDistance() - c->GetUSDistanceAux()) < 10.0 && c->GetUSDistance() < 250.0) {
        c->Turn(0);
        c->Forward(Speed::NONE);
        this->getProcedure() = Scout::FINDING_MOUNTAIN;
        return;
      }
    }

    c->Forward(Speed::NONE);
    this->getProcedure() = Scout::FINDING_MOUNTAIN;
    return;
  } else if(this->getProcedure() == Scout::FINDING_SAMPLE) {
    c->ToggleUSTurn(false);
    c->SetUSAngle(0);
    c->Forward(Speed::FULL);

    bool reachedBorder = false;

    while(!reachedBorder) { // Not reached border
      if(c->GetUSDistance() < 25.0) {
        // We encountered a mountain, go look behind it.
        c->Forward(Speed::NONE);
        this->getProcedure() = Scout::FINDING_MOUNTAIN;
        return;
      } else if(c->GetIRLeft() == Infrared::BLACK || c->GetIRRight() == Infrared::BLACK) {
        if(!this->avoid(c)) {
          reachedBorder = true;
        }
      } else if(c->GetUSDistanceAux() < 15.0) {
        c->Forward(Speed::HALF);
        while(c->GetUSDistanceAux() > 8.0);
        c->Forward(Speed::NONE);

        c->Grab(true); // IR sample detection
        delay(300);

        c->Forward(Speed::HALF);
        if(c->GetIRSample() == Infrared::WHITE) {
          c->Grab(false);
          delay(500);
          c->Grab(true);

          this->getProcedure() = Scout::RETURNING_LAB;
        } else {
          c->Grab(false);
          this->getProcedure() = Scout::SWEEP;
        }

        c->Forward(Speed::NONE);
        return;
      }
    }

    c->Turn(180);
    while(c->IsPerforming(Action::TURNING_RIGHT));
    c->Forward(Speed::NONE);

    this->getProcedure() = Scout::FINDING_MOUNTAIN;
    return;
  } else if(this->getProcedure() == Scout::FINDING_MOUNTAIN) {
    c->ToggleUSTurn(true);
    c->Forward(Speed::FULL);

    bool reachedBorder = false;

    while(!reachedBorder) {
      if(c->GetIRLeft() == Infrared::BLACK || c->GetIRRight() == Infrared::BLACK) {
        if(!this->avoid(c)) {
          reachedBorder = true;
        }
      } else if(c->GetUSDistance() < 25.0) {
        c->Forward(Speed::NONE);
        c->Turn(c->GetUSAngle()); // Turn towards the mountain
        c->ToggleUSTurn(false);
        while(c->IsPerforming(Action::TURNING_LEFT) || c->IsPerforming(Action::TURNING_RIGHT));

        bool foundPassage = false;
        bool left = true;
        short count = 0;
        double distance = 0.0;
        c->ResetTravelDist();

        c->Forward(Speed::FULL);
        c->Turn(-90);
        while(c->IsPerforming(Action::TURNING_LEFT));

        c->Forward(Speed::NONE);
        delay(200); // Give the sensor some time to acquire data
        if(c->GetUSDistance() < 25.0) {
          left = false;
          c->Turn(180);
          while(c->IsPerforming(Action::TURNING_RIGHT));
        }
        c->Forward(Speed::FULL);

        unsigned long startTime = millis();

        while(count < 3 && (startTime - millis()) < (20 * 1000)) {  // Timeout of 20s
          unsigned long startDriveTime = millis();
          while((startDriveTime - millis()) < 500) {
            if(c->GetIRLeft() == Infrared::BLACK || c->GetIRRight() == Infrared::BLACK) {
              if(!this->avoid(c)) {
                c->Turn(180);
                while(c->IsPerforming(Action::TURNING_RIGHT));

                c->Forward(Speed::NONE);
                this->getProcedure() == Scout::SWEEP;
                return;
              }
            }
          }

          c->Turn(left ? 90 : -90); // Turn back to look at the mountain
          while(left ? c->IsPerforming(Action::TURNING_RIGHT) : c->IsPerforming(Action::TURNING_LEFT));

          c->Forward(Speed::NONE);
          delay(200);
          if(c->GetUSDistance() > 25.0) {
            foundPassage = true;
            break;
          }
          c->Forward(Speed::FULL);

          c->Turn(left ? -90 : 90);
          while(left ? c->IsPerforming(Action::TURNING_LEFT) : c->IsPerforming(Action::TURNING_RIGHT));

          count++;
        }

        if(foundPassage) {
          distance = c->GetTravelDist();
          c->SetUSAngle(left ? 90 : -90);
          delay(1500); // Allow some time for the US turning and updating

          unsigned long startDriveTime = millis();

          while(c->GetUSDistance() < 25.0 && (millis() - startDriveTime) < (10 * 1000)) {
            if(c->GetIRLeft() == Infrared::BLACK || c->GetIRRight() == Infrared::BLACK) {
              if(!this->avoid(c)) {
                c->Turn(180);
                while(c->IsPerforming(Action::TURNING_RIGHT));

                c->Forward(Speed::NONE);
                this->getProcedure() == Scout::SWEEP;
                return;
              }
            }
          }

          c->SetUSAngle(0);

          c->Turn(left ? 90 : -90);
          while(left ? c->IsPerforming(Action::TURNING_RIGHT) : c->IsPerforming(Action::TURNING_LEFT));

          startDriveTime = millis();
          c->ResetTravelDist();
          while(c->GetTravelDist() < distance && (millis() - startDriveTime) < (10 * 1000)) {
            if(c->GetIRLeft() == Infrared::BLACK || c->GetIRRight() == Infrared::BLACK) {
              if(!this->avoid(c)) {
                c->Turn(180);
                while(c->IsPerforming(Action::TURNING_RIGHT));

                c->Forward(Speed::NONE);
                this->getProcedure() == Scout::SWEEP;
                return;
              }
            }
          }
        }

        c->Forward(Speed::NONE);
        this->getProcedure() = Scout::SWEEP;
        return;
      }
    }

    c->Turn(180);
    while(c->IsPerforming(Action::TURNING_RIGHT));
    c->Forward(Speed::NONE);

    this->getProcedure() = Scout::SWEEP;
    return;
  } else if(this->getProcedure() == Scout::RETURNING_LAB) {
    c->ToggleUSTurn(true);

    short count = 0;
    bool foundLab = false;
    unsigned long startSearchTime;

    while(count < 10 && !foundLab) {
      c->Forward(Speed::FULL);
      c->Turn(180);
      while(c->IsPerforming(Action::TURNING_RIGHT));
      c->Forward(Speed::NONE);

      startSearchTime = millis();

      while((millis() - startSearchTime) < (25 * 1000)) {
        if(c->GetIRLab() == Infrared::WHITE) {
          c->Forward(Speed::FULL);
          c->Turn(c->GetUSAngle()); // Turn towards the lab
          c->ToggleUSTurn(false);
          while(c->IsPerforming(Action::TURNING_LEFT) || c->IsPerforming(Action::TURNING_RIGHT));
          c->Forward(Speed::NONE);

          foundLab = true;
          break;
        }
      }
    }

    if(foundLab) {
      // todo
    }

    c->Forward(Speed::NONE);
    this->getProcedure() = Scout::SWEEP;
}
