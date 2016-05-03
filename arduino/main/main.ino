/*
 *  TU/e Venus OGO
 *  Main
 *  Arduino Logic
 */

#include "RobotController.h"

RobotController* robotController;

void setup() {
	// Debug
	Serial.begin(9600);

	// Initialize robotController
	robotController = new RobotController();

	// Start moving
	robotController->Forward(Speed::FULL);
}

void loop() {

}
