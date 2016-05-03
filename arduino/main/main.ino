/*
 *  TU/e Venus OGO
 *  Main
 *  Arduino Logic
 */

#include "RobotController.h"
#include "Tests.h"
#include "Layout.h"
#include "TimerOne.h"
#include "ControlThread.h"

RobotController* robotController;
ControlThread* test1;
ControlThread* test2;
int count = 0;

void timerCallback() {
  controlThreadPool.run();
}

void testFunction1() {
  Serial.println("Hallo, thread 1 hier");
}

void testFunction2() {
  Serial.println("Hallo, thread 2 hier");
}

void setup() {
	// Debug
	Serial.begin(9600);
  Serial.println("Start debug");

	// Initialize robotController
	robotController = new RobotController();

  // Setup the test button
  pinMode(PIN_BTN_TEST, INPUT);

  // Initialize threads
  test1 = new ControlThread(&testFunction1);
  test2 = new ControlThread(&testFunction2);

  // Setup timer
  Timer1.initialize(1000 * 1000);
  Timer1.attachInterrupt(timerCallback);
}

void loop() {

}
