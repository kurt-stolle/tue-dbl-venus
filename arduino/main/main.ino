/*
 *  TU/e Venus OGO
 *  Main 
 *  Arduino Logic
 */

#include "controller.h"

Controller* ctrl;

void setup() {
  // Allocate a layout that we may fill out here
  ControllerLayout* layout = new ControllerLayout();
  
  layout->wheelLeft = 0;
  layout->wheelRight = 0;

  // Construct the controller
  ctrl = new Controller(layout);
}

void loop() {
  ctrl->Turn(-1);
}
