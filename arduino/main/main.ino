/*
 *  TU/e Venus OGO
 *  Main
 *  Arduino Logic
 */

#include "RobotController.h"
#include "Layout.h"
#include "TimerOne.h"
#include "ControlThread.h"
#include "Ultrasonic.h"

 // General variables
RobotController* robotController;
ControlThread* driveThread;
ControlThread* scanThread;
bool debugging = false;
bool debuggingFinished = false;
unsigned short count = 0;

// Interrupt functions
void driveCallback();
void scanCallback();
void timerCallback();
void echoCallback();

// Arduino functions
void setup();
void loop();

/*
 * Setup & Thread control
 */

void setup() {
	/* Sets up serial debugging, intializes a robot, registers threads
	and their callbacks and creates a timer for the thread execution. */

	Serial.begin(9600);
	Serial.println("Start debug");

	robotController = new RobotController();

	driveThread = new ControlThread(driveCallback);
	scanThread = new ControlThread(scanCallback);
	attachInterrupt(digitalPinToInterrupt(PIN_ECHO_ULTRASOUND), echoCallback, CHANGE);

	Timer1.initialize(10 * 1000);
	Timer1.attachInterrupt(timerCallback);
}

void timerCallback() {
	/* Calls the threadcontroller to check whether
	threads in the pool need to be executed at this moment. */

	controlThreadPool.run();
}

/*
 * Main logic and callback functions
 */

void loop() {
	/* Executes main algorithm, leaving fast concurrent tasks
	to the interrupt callbacks */

	// Test routine
	if (debugging) {
		if (debuggingFinished) {
			Serial.println("Debugging ended...");
			delay(5000);
			return;
		}

		robotController->Forward(Speed::FULL);
		while (robotController->getUSDistance() == -1 || robotController->getUSDistance() < 5.0) {}
		robotController->Reverse(Speed::HALF);
		delay(500);
		robotController->Turn(-90);
		delay(500);
		robotController->Reverse(Speed::NONE);
		delay(500);
		robotController->Turn(0);

		count++;

		if (count >= 3) {
			debuggingFinished = true;
		}
	}
}

void driveCallback() {
	/* Handles all tasks concerning the driving state of
	the robot (updating servo acceleration etc.) */

	if (debugging) {
		Serial.println("driveCallback thread");
	}

	// TODO
}

void scanCallback() {
	/* Sends a pulse from the main ultrasonic sensor
	on the head of the robot */

	if (debugging) {
		Serial.println("scanCallback thread");
	}

	robotController->Scan();
}

void echoCallback() {
	/* Listens to a response from the robot's US sensor
	and sets its distance variable to a new value */

	if (debugging) {
		Serial.println("ultrasonic echo pin change");
	}

	robotController->USListen();
}