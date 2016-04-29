/*
 *  TU/e Venus OGO
 *  Main 
 *  Arduino Logic
 */

#include "Controller.h"

Controller* ctrl;

void setup() {
  // Construct the controller
  ctrl = new Controller();
}

void loop() {
  ctrl->Turn(-1);
}
