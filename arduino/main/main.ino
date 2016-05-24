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

/*
 * Possible algorithms:
 * 1 - Debug mode (testing routine)
 * 2 - Calibration mode (sets the wheels to move at speed 0 without disabling them)
 * 3 - Venus mode (venus exploration routine)
 */
#define ALGORITHM 1


/*
 * Algorithm specific variables
 */
#if ALGORITHM == 2
Servo wheelLeft, wheelRight;
#endif

/*
 * General variables
 */
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
  Serial.begin(9600);

  // Serial should not be possible in Venus mode
  // Actually it should, because wireless comm. needs it
#if ALGORITHM == 1
  Serial.println("Starting in DEBUG mode");
#elif ALGORITHM == 2
  wheelLeft.attach(PIN_MOTOR_LEFT);
  wheelRight.attach(PIN_MOTOR_RIGHT);
  
  Serial.println("Starting in CALIBRATION mode");
#endif
  
  // Create an instance for the robot controller
	robotController = new RobotController();

  // Setup threads
	driveThread = new ControlThread(driveCallback);
	scanThread = new ControlThread(scanCallback);

  // Attach an interrupt for the ultrasound
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

#if ALGORITHM == 1 /* Debug mode */
    
		robotController->Forward(Speed::FULL);

    robotController->Grab(true);
    
  	while (robotController->GetUSDistance() > 10.0) { 
      delay(300);
		}

    robotController->Grab(false);

    robotController->Forward(Speed::NONE);

    delay(300);
    
    robotController->Forward(Speed::FULL);
    robotController->Turn(90);

    delay(5 * 1000);
    
#elif ALGORITHM == 2 /* Calibration mode */

  wheelLeft.write(1500);
  wheelRight.write(1500);

  delay(100000);
  
#elif ALGORITHM == 3 /* Venus mode */

  //robotController->Forward(Speed::FULL);
  
#endif
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

  robotController->USListen();
}
