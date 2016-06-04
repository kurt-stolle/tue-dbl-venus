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
#include "Infrared.h"
#include "Algorithm.h"

/*
 * Possible algorithms:
 * 1 - Debug mode (testing routine)
 * 2 - Calibration mode (prints sensor debug)
 * 3 - Scout mode (venus exploration routine)
 * 4 - Foraging mode (lab collector routine)
 */
#define ALGORITHM 3

/*
 * Algorithm specific variables
 */
#if ALGORITHM == 3
namespace Procedure {
  enum Procedure {
    SWEEP,
    FINDING_SAMPLE,
    FINDING_MOUNTAIN,
    RETURNING_LAB
  };
}

Procedure::Procedure doing = Procedure::SWEEP;
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
void avoidCliff();

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
    while(robotController->GetUSDistance() > 20.0);
    robotController->Reverse(Speed::HALF);
    delay(1000);
    robotController->Forward(Speed::FULL);
    robotController->Turn(-45);
    while(robotController->IsPerforming(Action::TURNING_LEFT));
    robotController->Forward(Speed::NONE);
    robotController->Grab(true);
    delay(2000);
    robotController->Grab(false);
    delay(5000);

#elif ALGORITHM == 2 /* Sensor calibration mode */

  Serial.print("Ultrasonic Main: "); Serial.println(robotController->GetUSDistance());
  Serial.print("Ulrasonic Aux: "); Serial.println(robotController->GetUSDistanceAux());
  //Serial.print("LineSensor Left: "); Serial.println(robotController->GetUSDistanceAux());
  //Serial.print("LineSensor Right: "); Serial.println(robotController->GetUSDistanceAux());
  //Serial.print("Long Range IR: "); Serial.println(robotController->GetUSDistanceAux());
  //Serial.print("Lab Finding IR: "); Serial.println(robotController->GetUSDistanceAux());

  delay(200);

#elif ALGORITHM == 3 /* Scout mode */

  Serial.print("Procedure: "); Serial.println(doing);

  if(doing == Procedure::SWEEP) {
    /*
     * Make a 90 degrees turn to the left and to the right.
     * If the bottom sensor detects something closer than the
     * top sensor while turning: stop and go to the find
     * sample procedure. Else: try to find a mountain (now
     * the sensors will detect the same).
     */
    robotController->ToggleUSTurn(false);
    robotController->SetUSAngle(0);
    robotController->Forward(Speed::HALF);
    robotController->Turn(-90);
    while(robotController->IsPerforming(Action::TURNING_LEFT)) {
      if((robotController->GetUSDistance() - robotController->GetUSDistanceAux()) > 10.0 && robotController->GetUSDistance() < 250.0) { // 3m for inacurracy protection
        robotController->Turn(0);
        robotController->Forward(Speed::NONE);
        doing = Procedure::FINDING_SAMPLE;
        return;
      }
    }

    robotController->Turn(180);
    while(robotController->IsPerforming(Action::TURNING_RIGHT)) {
      if((robotController->GetUSDistance() - robotController->GetUSDistanceAux()) > 10.0 && robotController->GetUSDistance() < 250.0) {
        robotController->Turn(0);
        robotController->Forward(Speed::NONE);
        doing = Procedure::FINDING_SAMPLE;
        return;
      }
    }

    // Try to find mountain
    robotController->Turn(-180);
    while(robotController->IsPerforming(Action::TURNING_LEFT)) {
      if((robotController->GetUSDistance() - robotController->GetUSDistanceAux()) < 10.0 && robotController->GetUSDistance() < 250.0) {
        robotController->Turn(0);
        robotController->Forward(Speed::NONE);
        doing = Procedure::FINDING_MOUNTAIN;
        return;
      }
    }

    robotController->Forward(Speed::NONE);
    doing = Procedure::FINDING_MOUNTAIN;
    return;
  } else if(doing == Procedure::FINDING_SAMPLE) {
    robotController->ToggleUSTurn(false);
    robotController->SetUSAngle(0);
    robotController->Forward(Speed::FULL);

    while() { // Not reached border (line sensors see black but LR infrared does not).
      if(robotController->GetUSDistance() < 25.0) {
        // We encountered a mountain, go look behind it.
        robotController->Forward(Speed::NONE);
        doing = Procedure::FINDING_MOUNTAIN;
        return;
      } else if(/* Long range IR sees cliff */) {
        avoidCliff();
      } else if(robotController->GetUSDistanceAux() < 15.0) { // And LR infrared sees white?
        robotController->Forward(Speed::HALF);
        while(robotController->GetUSDistanceAux() > 5.0); // And LR infrared sees white/black?
        robotController->Forward(Speed::NONE);

        robotController->Grab(true);

        doing = Procedure::RETURNING_LAB;
        return;
      }
    }

    robotController->Turn(180);
    while(robotController->IsPerforming(TURNING_RIGHT));
    robotController->Forward(Speed::NONE);

    doing = Procedure::FINDING_MOUNTAIN;
    return;
  } else if(doing == Procedure::FINDING_MOUNTAIN) {
    robotController->ToggleUSTurn(true);
    robotController->Forward(Speed::FULL);

    while() { // Not reached border
      // Avoid cliffs with LR infrared, but recover straight line after that
      if(/* Long range IR sees cliff */) {
        avoidCliff();
      } else if(robotController->GetUSDistance() < 25.0) {
        robotController->Turn(robotController->GetUSAngle()); // Turn towards the mountain
        robotController->ToggleUSTurn(false);
        while(robotController->IsPerforming(Action::TURNING_LEFT) || robotController->IsPerforming(Action::TURNING_RIGHT));

        bool foundPassage = false;
        bool left = true;
        short count = 0;
        double distance = 0.0;
        robotController->ResetDistTraveled();

        robotController->Turn(-90);
        while(robotController->IsPerforming(Action::TURNING_LEFT));

        robotController->Forward(Speed::NONE);
        delay(200); // Give the sensor some time to acquire data
        if(robotController->GetUSDistance() < 25) {
          left = false;
          robotController->Turn(180);
          while(robotController->IsPerforming(Action::TURNING_RIGHT));
        }
        robotController->Forward(Speed::FULL);

        while(count < 2) {
          delay(500); // Check for cliffs/boundary while driving?
          robotController->Turn(left ? 90 : -90); // Turn back to look at the mountain
          while(left ? robotController->IsPerforming(Action::TURNING_RIGHT) : robotController->IsPerforming(Action::TURNING_LEFT));

          robotController->Forward(Speed::NONE);
          delay(200);
          if(robotController->GetUSDistance() > 25.0) {
            foundPassage = true;
            break;
          }
          robotController->Forward(Speed::FULL);

          robotController->Turn(left : -90 : 90);
          while(left ? robotController->IsPerforming(Action::TURNING_LEFT) : robotController->IsPerforming(Action::TURNING_RIGHT));

          count++;
        }

        if(foundPassage) {
          distance = robotController->GetDistTraveled();
          robotController->SetUSAngle(left ? 90 : -90);
          while(robotController->GetUSDistance < 25.0); // add timeout here
          robotController->SetUSAngle(0);

          robotController->Turn(left ? 90 : -90);
          while(robotController->IsPerforming(Action::TURNING_RIGHT));

          robotController->ResetDistTraveled();
          while(robotController->GetDistTraveled() < distance); // AND no mountains/cliffs & timeout
        }

        robotController->Forward(Speed::NONE);
        doing = Procedure::SWEEP;
        return;
      }
    }

    robotController->Turn(180);
    while(robotController->IsPerforming(TURNING_RIGHT));
    robotController->Forward(Speed::NONE);

    doing = Procedure::SWEEP;
    return;
  } else if(doing == Procedure::RETURNING_LAB) {
    short count = 0;
    bool foundLab = false;

    while(count < 10) {
      robotController->ToggleUSTurn(true);
      //if(/* Sensor on top sees lab */) {
        robotController->Forward(Speed::FULL);
        robotController->Turn(robotController->GetUSAngle()); // Turn towards the lab
        robotController->ToggleUSTurn(false);
        while(robotController->IsPerforming(Action::TURNING_LEFT) || robotController->IsPerforming(Action::TURNING_RIGHT));
        robotController->Forward(Speed::NONE);

        foundLab = true;
        break;
      //}


      robotController->Forward(Speed::FULL);
      robotController->Turn(180);
      while(robotController->IsPerforming(TURNING_RIGHT));
      robotController->Forward(Speed::NONE);
    }

    if(foundLab) {
      // Drive towards lab while avoiding cliffs & the boundary
    }

    robotController->Forward(Speed::NONE);
    doing = Procedure::SWEEP;
  }


#elif ALGORITHM == 4 /* Collector mode */

  robotController->Forward(Speed::FULL);

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
