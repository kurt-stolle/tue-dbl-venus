#include "Algorithm.h"
#include "CollectorAlgorithm.h"
#include "Arduino.h"
#include "RobotController.h"
#include "Layout.h"

#define DISTANCE_CRITICAL 10.0
#define DISTANCE_INSIGNIFICANT 20.0

void CollectorAlgorithm::setup(RobotController* c) {
  this->setProcedure(Collector::SWEEPING);
}

void CollectorAlgorithm::loop(RobotController* c) {
  c->ToggleUSTurn(false); // Disable ultrasonic turning
  c->SetUSAngle(0); // Set the angle to 0
  c->Forward(Speed::FULL); // Full power ahead

  Serial.println("Moving forward");
  
  while(c->GetUSDistance() > 10) delay(50); // Wait until the distance becomes less than 10cm

  Serial.println("Mountain!!!!");

  // Stop moving
  c->Forward(Speed::NONE);
  
  // Look to the left
  c->SetUSAngle(-90);
  delay(CALIBRATION_TIME_US_TURN); // Wait for head to turn
  if (c->GetUSDistance() < DISTANCE_INSIGNIFICANT){
    Serial.println("Left not clear.");
    // Look to the right
    c->SetUSAngle(90);
    delay(CALIBRATION_TIME_US_TURN); // Wait for head to turn
    if (c->GetUSDistance() > DISTANCE_INSIGNIFICANT){
       Serial.println("Right clear! Moving right.");
       c->Forward(Speed::FULL);
       c->Turn(-90);
       while (c->IsPerforming(Action::TURNING)) delay(50); 
    } else {
      Serial.println("We're fucked. Returning.");
      c->Forward(Speed::FULL);
      c->Turn(180);
      while (c->IsPerforming(Action::TURNING)) delay(50);
    }    
  } else {
    Serial.println("Left clear. Moving left.");
    c->Forward(Speed::FULL);
    c->Turn(-90);
    while (c->IsPerforming(Action::TURNING)) delay(50); // Wait until we turned
  }
}

