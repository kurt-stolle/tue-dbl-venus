#include "Algorithm.h"
#include "CollectorAlgorithm.h"
#include "Arduino.h"
#include "RobotController.h"
#include "Layout.h"

#define DISTANCE_CRITICAL 10.0
#define DISTANCE_INSIGNIFICANT 20.0
#define TIME_MOVE_15CM 10.0
#define BLACKSTUFF_LEFT 0
#define BLACKSTUFF_RIGHT 1

void CollectorAlgorithm::setup(RobotController* c) {

  this->setProcedure(Collector::SWEEPING);
  this->whichSide = BLACKSTUFF_LEFT;
}

void CollectorAlgorithm::loop(RobotController* c) {
  c->Forward(Speed::NONE);

  switch (this->getProcedure()) {
    case Collector::SWEEPING:
      c->ToggleUSTurn(false); // Disable ultrasonic turning
      c->SetUSAngle(0); // Set the angle to 0
      c->Forward(Speed::FULL); // Full power ahead

      Serial.println("Moving forward");

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

      else if ((c->GetUSDistance() - c->GetUSDistanceAux()) > 10.0 && c->GetUSDistanceAux() < 20.0) {
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
      delay(CALIBRATION_TIME_US_TURN);

      if (c->GetUSDistance() > DISTANCE_INSIGNIFICANT){
        Serial.println("Left is free");
        
        c->Forward(Speed::HALF);
        c->Turn(-90);

        while(c->IsPerforming(Action::TURNING)) delay(5);
        
        this->setProcedure(Collector::SWEEPING);

        Serial.println("Continuing left");
        
        return;
      }

      // Right free?
      c->SetUSAngle(90);
      delay(CALIBRATION_TIME_US_TURN);

      if (c->GetUSDistance() > DISTANCE_INSIGNIFICANT) {
        Serial.println("Right is free, turning but moving slightly right");
       c->Forward(Speed::HALF);
       c->Turn(90);

       while(c->IsPerforming(Action::TURNING)) delay(5);
       
       c->Forward(Speed::FULL);

       delay(TIME_MOVE_15CM);

       c->Turn(90);
       while(c->IsPerforming(Action::TURNING)) delay(5);

       this->setProcedure(Collector::SWEEPING);

       Serial.println("Continuing in opposite direction shifted right");

       return;
      }

      Serial.println("Nothing is free!");

      c->Forward(Speed::FULL_REVERSE);
      
      delay(TIME_MOVE_15CM);
      
      break;

    case Collector::AVOIDING_CLIFF:
      this->setProcedure(Collector::SWEEPING);
      break;

    case Collector::GET_SAMPLE:
/*
      while (c->GetUSDistanceAux() > 10.0) {
        //Driving to sample
        delay(5);
      }

      unsigned long startDriveTime = millis();

      c->Forward(Speed::QUARTER);
      while (c->GetUSDistanceAux() > 6.0 && (millis() - startDriveTime) < 3000);
      c->Forward(Speed::NONE);

      c->Grab(true); // IR sample detection

      this->setProcedure(Collector::RETURNING_LAB);
      */
      
      break;

    case Collector::RETURNING_LAB:

      break;
  }
}

