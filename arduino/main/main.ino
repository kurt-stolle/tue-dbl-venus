/*
 *  TU/e Venus OGO
 *  Main 
 *  Arduino Logic
 */

#include "RobotController.h"

RobotController* robotController;

void setup() {
  // Initialize robotController
  robotController = new RobotController();

  Serial.begin(9600);

  // Start moving
  robotController->Accelerate();

}

void loop() {
  
}
