#include "Algorithm.h"
#include "CollectorAlgorithm.h"
#include "Arduino.h"
#include "RobotController.h"
#include "Layout.h"

#define DISTANCE_CRITICAL 20.0
#define DISTANCE_INSIGNIFICANT 25.0
#define TIME_MOVE_15CM 10.0
#define BLACKSTUFF_LEFT 0
#define BLACKSTUFF_RIGHT 1
#define LOOK_WAIT_TIME 1000

void CollectorAlgorithm::setup(RobotController* c) {

  this->setProcedure(Collector::SWEEPING);
  this->whichSide = BLACKSTUFF_LEFT;
}

void CollectorAlgorithm::loop(RobotController* c) {
  c->Forward(Speed::NONE);

  switch (this->getProcedure()) {
    case Collector::SWEEPING:
      c->ToggleUSTurn(true); // Disable ultrasonic turning
      c->Forward(Speed::FULL); // Full power ahead
      
      if (c->GetUSDistance() < DISTANCE_CRITICAL) {
        // Distance of US is less than 10, mountain is detected.
        Serial.println("Mountain detected bitch");

        this->setProcedure(Collector::AVOIDING_MOUNTAIN);
        return;
      }
      else if (c->GetIRRight() == Infrared::BLACK) {
        Serial.println("BLACK stuff Right");

        whichSide = BLACKSTUFF_RIGHT;

        this->setProcedure(Collector::AVOIDING_CLIFF);
        return;

      }
      else if (c->GetIRLeft() == Infrared::BLACK) {
        Serial.println("BLACK stuff Left");
       
        whichSide = BLACKSTUFF_LEFT;

        this->setProcedure(Collector::AVOIDING_CLIFF);
        return;

      }

      
      // No mountains, no cliffs, look for sample
      if (c->GetUSDistance() < 500 && c->GetUSDistanceAux() < 500 && ((c->GetUSDistance() - c->GetUSDistanceAux()) > 10.0 && c->GetUSDistanceAux() < 10.0)) {
        //
        Serial.println("Detected sample");

        this->setProcedure(Collector::GET_SAMPLE);
        return;
      }

      break;

    case Collector::AVOIDING_MOUNTAIN:
      c->ToggleUSTurn(false);

      // Left free?
      c->SetUSAngle(-90);
      delay(LOOK_WAIT_TIME);

      if (c->GetUSDistance() > DISTANCE_INSIGNIFICANT) {
        Serial.println("Left is free");

        c->Forward(Speed::HALF);
        c->Turn(-90);

        while (c->IsPerforming(Action::TURNING)) {
          Serial.println("Turing LEFT");
          delay(5);
        }

        this->setProcedure(Collector::SWEEPING);

        Serial.println("Continuing left");

        return;
      }

      // Right free?
      c->SetUSAngle(90);
      delay(LOOK_WAIT_TIME);

      if (c->GetUSDistance() > DISTANCE_INSIGNIFICANT) {
        Serial.println("Right is free, turning but moving slightly right");
        c->Forward(Speed::HALF);
        c->Turn(90);

        while (c->IsPerforming(Action::TURNING)) {
          Serial.println("Turing right");
          delay(5);
        }

        c->Forward(Speed::FULL);

        c->ResetTravelDist();
        while (c->GetTravelDist() < 5) continue;

        c->Turn(90);
        while (c->IsPerforming(Action::TURNING)) delay(5);

        this->setProcedure(Collector::SWEEPING);

        Serial.println("Continuing in opposite direction shifted right");

        return;
      }

      Serial.println("Nothing is free!");

      c->Forward(Speed::FULL_REVERSE);

      c->ResetTravelDist();
      while (c->GetTravelDist() < 15) continue;

      c->Turn(180);

      while (c->IsPerforming(Action::TURNING)) continue;

      this->setProcedure(Collector::SWEEPING);

      break;

    case Collector::AVOIDING_CLIFF:

      c->ResetTravelDist();
      c->Forward(Speed::FULL_REVERSE);
      while (c->GetTravelDist() < 3) continue;

      if (whichSide == BLACKSTUFF_LEFT) {
        Serial.println("Right is free, turning but moving slightly right");
        c->Forward(Speed::HALF);
        c->Turn(90);

        while (c->IsPerforming(Action::TURNING)) {
          Serial.println("Turing right");
          delay(5);
        }

        c->Forward(Speed::FULL);

        c->ResetTravelDist();
        while (c->GetTravelDist() < 15) continue;
        

        c->Turn(90);
        while (c->IsPerforming(Action::TURNING)) delay(5);

        this->setProcedure(Collector::SWEEPING);

        Serial.println("Continuing in opposite direction shifted right");

        return;
      } else if (whichSide == BLACKSTUFF_RIGHT) {

        c->ResetTravelDist();
        c->Forward(Speed::FULL_REVERSE);
        while (c->GetTravelDist() < 3) continue;


        Serial.println("Left is free");

        c->Forward(Speed::HALF);
        c->Turn(-90);

        this->setProcedure(Collector::SWEEPING);

        Serial.println("Continuing left");

        return;

      }
      this->setProcedure(Collector::SWEEPING);
      break;

    case Collector::GET_SAMPLE:
      c->Forward(Speed::NONE);

      c->Grab(true);

      delay(1000);


      this->setProcedure(Collector::RETURNING_LAB);

      break;

    case Collector::RETURNING_LAB:

      Serial.println("Returning to lab");
    
      this->returnToLab(c);
      break;
  }
}

