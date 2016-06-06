/*
 *  TU/e Venus OGO
 *  Main
 *  Arduino Logic
 */

 /*
  * Possible algorithms:
  * - CalibrationAlgorithm
  * - ScoutAlgorithm
  * - CollectorAlgorithm
  */
 #define ALGORITHM ScoutAlgorithm

 /*
  * Includes
  */

#include "RobotController.h"
#include "Layout.h"
#include "ControlThread.h"
#include "Ultrasonic.h"
#include "TimerOne.h"
#include "Infrared.h"

#if ALGORITHM == CalibrationAlgorithm
  #include "CalibrationAlgorithm.h"
#elif ALGORITHM == ScoutAlgorithm
  #include "ScoutAlgorithm.h"
#elif ALGORITHM == CollectorAlgorithm
  #include "CollectorAlgorithm.h"
#endif

/*
 * General variables
 */
RobotController* robotController;
ControlThread* driveThread;
ControlThread* scanThread;
ControlThread* commThread;

// Interrupt functions
void driveCallback();
void scanCallback();
void timerCallback();
void commCallback();
void echoCallback();
void echoAuxCallback();

// Arduino functions
void setup();
void loop();
bool avoid();

// Algorithm
ALGORITHM * alg;

/*
 * Setup & Thread control
 */

void setup() {
  Serial.begin(9600);

  // Create an instance for the robot controller
	robotController = new RobotController();

  // Setup threads
	driveThread = new ControlThread(driveCallback);
	scanThread = new ControlThread(scanCallback);
  //commThread = new ControlThread(commCallback);

  // Attach an interrupt for the ultrasound
	attachInterrupt(digitalPinToInterrupt(PIN_ECHO_ULTRASOUND), echoCallback, CHANGE);
  attachInterrupt(digitalPinToInterrupt(PIN_ECHO_ULTRASOUNDAUX), echoAuxCallback, CHANGE);

  Timer1.initialize(100 * 1000L);
  Timer1.attachInterrupt(timerCallback);

  // Setup algo
  alg = new ALGORITHM ();
  alg->setup(robotController);
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
  if(*alg){
    alg->loop(robotController);
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

void commCallback() {
  robotController->Communicate();
}

void echoCallback() {
	/* Listens to a response from the robot's US sensor
	and sets its distance variable to a new value */

  robotController->USListen();
}

void echoAuxCallback() {
  /* Listens to a response from the robot's US sensor
  and sets its distance variable to a new value */

 robotController->USListenAux();
}
