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

  // Start moving
  robotController->Accelerate();
}

void loop() {
  
}
