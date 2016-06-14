#include "Algorithm.h"
#include "ScoutAlgorithm.h"
#include "Arduino.h"
#include "RobotController.h"

void ScoutAlgorithm::setup(RobotController* c) {
  this->setProcedure(Scout::FINDING_SAMPLE);
}

void ScoutAlgorithm::loop(RobotController* c) {

  Serial.print("Procedure: ");

  if (this->getProcedure() == Scout::SWEEP) {
    Serial.println("Sweep"); delay(10000);
    
    c->ToggleUSTurn(false);
    c->SetUSAngle(0);
    c->Forward(Speed::QUARTER);

    Serial.println("trying sample");

    c->Turn(-90);
    while (c->IsPerforming(Action::TURNING_LEFT)) {            
      if ((c->GetUSDistance() - c->GetUSDistanceAux()) > 6.0 && c->GetUSDistance() < 250.0 && c->GetUSDistanceAux() > 10.0) { // 3m for inacurracy protection
        Serial.print("Main: "); Serial.println(c->GetUSDistance());
        Serial.print("Aux: "); Serial.println(c->GetUSDistanceAux());
        
        c->Turn(0);
        c->Forward(Speed::NONE);
        this->setProcedure(Scout::FINDING_SAMPLE);
        return;
      }
    }

    Serial.println("trying sample once more");

    c->Turn(180);
    while (c->IsPerforming(Action::TURNING_RIGHT)) {      
      if ((c->GetUSDistance() - c->GetUSDistanceAux()) > 6.0 && c->GetUSDistance() < 250.0 && c->GetUSDistanceAux() > 10.0) { // 3m for inacurracy protection
        c->Turn(0);
        c->Forward(Speed::NONE);
        this->setProcedure(Scout::FINDING_SAMPLE);
        return;
      }
    }

    Serial.println("trying mountain");

    // Try to find mountain
    c->Turn(-180);
    while (c->IsPerforming(Action::TURNING_LEFT)) {      
      if ((c->GetUSDistance() - c->GetUSDistanceAux()) < 6.0 && c->GetUSDistance() < 250.0) {
        c->Turn(0);
        c->Forward(Speed::NONE);
        this->setProcedure(Scout::FINDING_MOUNTAIN);
        return;
      }
    }

    c->Forward(Speed::NONE);
    this->setProcedure(Scout::FINDING_MOUNTAIN);
    return;
  } else if (this->getProcedure() == Scout::FINDING_SAMPLE) {
    Serial.println("Sample"); delay(10000);

    /*c->ToggleUSTurn(false);
    c->SetUSAngle(0);
    c->Forward(Speed::FULL);

    bool reachedBorder = false;

    while (!reachedBorder) { // Not reached border
      if (c->GetUSDistance() < 25.0) {
        // We encountered a mountain, go look behind it.
        Serial.println("Detected mountain");
        c->Forward(Speed::NONE);
        this->setProcedure(Scout::FINDING_MOUNTAIN);
        return;
      } else if (c->GetIRLeft() == Infrared::BLACK || c->GetIRRight() == Infrared::BLACK) {
        Serial.println("black shit");
        
        if (!this->avoid(c)) {
          Serial.println("Reached border");
          reachedBorder = true;
        }
      } else if (c->GetUSDistanceAux() < 20.0 && c->GetUSDistanceAux() > 10.0) {
        Serial.println("Detected sample");

        unsigned long startDriveTime = millis();
        
        c->Forward(Speed::HALF);
        while (c->GetUSDistanceAux() > 6.0 && (millis() - startDriveTime) < 3000);
        c->Forward(Speed::NONE);

        c->Grab(true); // IR sample detection

        this->setProcedure(Scout::RETURNING_LAB);
        c->Forward(Speed::NONE);
        return;
      }
    }

    c->Turn(180);
    while (c->IsPerforming(Action::TURNING_RIGHT));
    c->Forward(Speed::NONE);

    this->setProcedure(Scout::FINDING_MOUNTAIN);
    return;*/
  } else if (this->getProcedure() == Scout::FINDING_MOUNTAIN) {
    Serial.println("Mountain"); delay(10000);
    
    c->ToggleUSTurn(true);
    c->Forward(Speed::FULL);

    bool reachedBorder = false;

    while (!reachedBorder) {
      if (c->GetIRLeft() == Infrared::BLACK || c->GetIRRight() == Infrared::BLACK) {
        if (!this->avoid(c)) {
          reachedBorder = true;
        }
      } else if (c->GetUSDistance() < 25.0) {
        c->Forward(Speed::NONE);
        c->Turn(c->GetUSAngle()); // Turn towards the mountain
        c->ToggleUSTurn(false);
        c->SetUSAngle(0);
        while (c->IsPerforming(Action::TURNING_LEFT) || c->IsPerforming(Action::TURNING_RIGHT));

        bool foundPassage = false;
        bool left = true;
        short count = 0;
        double distance = 0.0;
        c->ResetTravelDist();

        c->Forward(Speed::FULL);
        c->Turn(-90);
        while (c->IsPerforming(Action::TURNING_LEFT));

        c->Forward(Speed::NONE);
        delay(200); // Give the sensor some time to acquire data
        if (c->GetUSDistance() < 25.0) {
          left = false;
          c->Turn(180);
          while (c->IsPerforming(Action::TURNING_RIGHT));
        }
        c->Forward(Speed::FULL);

        unsigned long startTime = millis();

        while (count < 3 && (millis() - startTime) < (20 * 1000)) { // Timeout of 20s
          unsigned long startDriveTime = millis();
          while ((millis() - startDriveTime) < 500) {
            if (c->GetIRLeft() == Infrared::BLACK || c->GetIRRight() == Infrared::BLACK) {
              if (!this->avoid(c)) {
                c->Turn(180);
                while (c->IsPerforming(Action::TURNING_RIGHT));

                c->Forward(Speed::NONE);
                this->setProcedure(Scout::SWEEP);
                return;
              }
            }
          }

          c->Turn(left ? 90 : -90); // Turn back to look at the mountain
          while (c->IsPerforming(Action::TURNING_LEFT) || c->IsPerforming(Action::TURNING_RIGHT));

          c->Forward(Speed::NONE);
          delay(200);
          if (c->GetUSDistance() > 25.0) {
            foundPassage = true;
            break;
          }
          c->Forward(Speed::FULL);

          c->Turn(left ? -90 : 90);
          while (c->IsPerforming(Action::TURNING_LEFT) || c->IsPerforming(Action::TURNING_RIGHT));

          count++;
        }

        if (foundPassage) {
          distance = c->GetTravelDist();
          c->SetUSAngle(left ? 90 : -90);
          delay(1500); // Allow some time for the US turning and updating

          unsigned long startDriveTime = millis();

          while (c->GetUSDistance() < 25.0 && (millis() - startDriveTime) < (10 * 1000)) {
            if (c->GetIRLeft() == Infrared::BLACK || c->GetIRRight() == Infrared::BLACK) {
              if (!this->avoid(c)) {
                c->Turn(180);
                while (c->IsPerforming(Action::TURNING_RIGHT));

                c->Forward(Speed::NONE);
                this->setProcedure(Scout::SWEEP);
                return;
              }
            }
          }

          c->SetUSAngle(0);

          c->Turn(left ? 90 : -90);
          while (c->IsPerforming(Action::TURNING_LEFT) || c->IsPerforming(Action::TURNING_RIGHT));

          startDriveTime = millis();
          c->ResetTravelDist();
          while (c->GetTravelDist() < distance && (millis() - startDriveTime) < (10 * 1000)) {
            if (c->GetIRLeft() == Infrared::BLACK || c->GetIRRight() == Infrared::BLACK) {
              if (!this->avoid(c)) {
                c->Turn(180);
                while (c->IsPerforming(Action::TURNING_RIGHT));

                c->Forward(Speed::NONE);
                this->setProcedure(Scout::SWEEP);
                return;
              }
            }
          }
        }

        c->Forward(Speed::NONE);
        this->setProcedure(Scout::SWEEP);
        return;
      }
    }

    c->Turn(180);
    while (c->IsPerforming(Action::TURNING_RIGHT));
    c->Forward(Speed::NONE);

    this->setProcedure(Scout::SWEEP);
    return;
  } else if (this->getProcedure() == Scout::RETURNING_LAB) {
    Serial.println("Returning to lab"); delay(10000);
    
    c->ToggleUSTurn(true);

    short count = 0;
    bool foundLab = false;
    unsigned long startSearchTime;

    while (count < 10 && !foundLab) {
      c->Forward(Speed::FULL);
      c->Turn(180);
      while (c->IsPerforming(Action::TURNING_RIGHT));
      c->Forward(Speed::NONE);

      startSearchTime = millis();

      while ((millis() - startSearchTime) < (25 * 1000)) {
        if (c->GetIRLab() == Infrared::WHITE) {
          c->Forward(Speed::FULL);
          c->Turn(c->GetUSAngle()); // Turn towards the lab
          c->ToggleUSTurn(false);
          while (c->IsPerforming(Action::TURNING_LEFT) || c->IsPerforming(Action::TURNING_RIGHT));
          c->Forward(Speed::NONE);

          foundLab = true;
          break;
        }
      }

      count++;
    }

    bool reachedLab = false;

    if (foundLab) {
      // TODO
    } else {
      c->Forward(Speed::FULL);
      c->Turn(rand() % 180 - 90);
      while(c->IsPerforming(Action::TURNING_LEFT) || c->IsPerforming(Action::TURNING_RIGHT));

      unsigned long startDriveTime = millis();
      while((millis() - startDriveTime) < 10000) {
        if(c->GetIRLeft() == Infrared::BLACK || c->GetIRRight() == Infrared::BLACK || c->GetUSDistance() < 25.0) {
          c->Forward(Speed::NONE);
          return;
        }
      }
    }

    c->Forward(Speed::NONE);
    this->setProcedure(Scout::SWEEP);
    return;
  }
}
