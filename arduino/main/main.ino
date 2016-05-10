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
unsigned long lastPulse;

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

  Timer1.initialize(10000 * 1000);
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
    Serial.println("Starting test routine");
    
		if (debuggingFinished) {
			Serial.println("DEBUG ended...");
			delay(5000);
			return;
		}

		robotController->Forward(Speed::FULL);
		while (robotController->getUSDistance() > 5.0) {
		  //Serial.println(robotController->getUSDistance());  
		}
    delay(5000);
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

	if (DEBUG) {
		Serial.println("driveCallback thread");
	}

	robotController->UpdateMovement();
}

void scanCallback() {
	/* Sends a pulse from the main ultrasonic sensor
	on the head of the robot */

	if (DEBUG) {
		Serial.println("scanCallback thread");
	}

	robotController->Scan();
  lastPulse = micros();
}

void echoCallback() {
	/* Listens to a response from the robot's US sensor
	and sets its distance variable to a new value */

  Serial.print("Pin is: ");
  Serial.println(digitalRead(PIN_ECHO_ULTRASOUND));

	if (DEBUG) {
		Serial.println("ultrasonic echo pin change");
	}

  //robotController->USListen();
}
