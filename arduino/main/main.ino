/*
 *  TU/e Venus OGO
 *  Main
 *  Arduino Logic
 */

#include "RobotController.h"
#include "Tests.h"
#include "Layout.h"

RobotController* robotController;

void setup() {
	// Debug
	Serial.begin(9600);

	// Initialize robotController
	robotController = new RobotController();

  // Setup the test button
  pinMode(PIN_BTN_TEST, INPUT);
}

void loop() {
  // Wait for test button
  if (digitalRead(PIN_BTN_TEST) == HIGH){
    Tests::Run();
  }
}
