/*
 *  TU/e Venus OGO
 *  Main
 *  Arduino Logic
 */

#include "RobotController.h"
#include "Layout.h"
#include "ControlThread.h"
#include "Ultrasonic.h"
#include "TimerOne.h"

#define DEBUG true

 // General variables
RobotController* robotController;
ControlThread* driveThread;
ControlThread* scanThread;
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
	/* Sets up serial DEBUG, intializes a robot, registers threads
	and their callbacks and creates a timer for the thread execution. */
	Serial.begin(9600);
	Serial.println("Start debug");

	robotController = new RobotController();

	driveThread = new ControlThread(driveCallback);
	scanThread = new ControlThread(scanCallback);
  
	attachInterrupt(digitalPinToInterrupt(PIN_ECHO_ULTRASOUND), echoCallback, CHANGE);

  Timer1.initialize(100 * 1000L);
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
	if (DEBUG) {   
		if (debuggingFinished) {
			Serial.println("DEBUG ended...");
			delay(5000);
			return;
		}

    Serial.println("Starting test routine");

		robotController->Forward(Speed::FULL);
		while (robotController->GetUSDistance() > 10.0) { 
      delay(300);
		}
    robotController->Forward(Speed::NONE);
    delay(1000);

		count++;

		if (count >= 3) {
			debuggingFinished = true;
		}
	}
}

void driveCallback() {
	/* Handles all tasks concerning the driving state of
	the robot (updating servo acceleration etc.) */

	robotController->UpdateMovement();
}

void scanCallback() {
	/* Sends a pulse from the main ultrasonic sensor
	on the head of the robot */

	robotController->Scan();
}

void echoCallback() {
	/* Listens to a response from the robot's US sensor
	and sets its distance variable to a new value */

  //Serial.println(digitalRead(2));

	/*if (DEBUG) { DON'T CALL LIBRARY FUNCTIONS IN INTERRUPTS
		Serial.println("ultrasonic echo pin change");
	}*/

  robotController->USListen();
}
