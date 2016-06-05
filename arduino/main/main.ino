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
bool avoid();

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

  //Serial.print("Ultrasonic Main: "); Serial.println(robotController->GetUSDistance());
  //Serial.print("Ulrasonic Aux: "); Serial.println(robotController->GetUSDistanceAux());
  //Serial.print("LineSensor Left: "); Serial.println(robotController->GetUSDistanceAux());
  //Serial.print("LineSensor Right: "); Serial.println(robotController->GetUSDistanceAux());
  //Serial.print("Long Range IR: "); Serial.println(robotController->GetUSDistanceAux());
  //Serial.print("Lab Finding IR: "); Serial.println(robotController->GetUSDistanceAux());

  delay(200);

#elif ALGORITHM == 3 /* Scout mode */

  // For quick debug
  String procedureName;

  switch(doing) {
    case Procedure::SWEEP:
      procedureName = "sweeping";
      break;
    case Procedure::FINDING_SAMPLE:
      procedureName = "finding sample";
      break;
    case Procedure::FINDING_MOUNTAIN:
      procedureName = "finding mountain";
      break;
    case Procedure::RETURNING_LAB:
      procedureName = "returning to lab";
      break;
    default:
      procedureName = "unknown";
  }
  
  Serial.print("Procedure: "); Serial.println(procedureName);

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

    bool reachedBorder = false;

    while(!reachedBorder) { // Not reached border
      if(robotController->GetUSDistance() < 25.0) {
        // We encountered a mountain, go look behind it.
        robotController->Forward(Speed::NONE);
        doing = Procedure::FINDING_MOUNTAIN;
        return;
      } else if(robotController->GetIRLeft() == Infrared::BLACK || robotController->GetIRRight() == Infrared::BLACK) {
        if(!avoid()) {
          reachedBorder = true;
        }
      } else if(robotController->GetUSDistanceAux() < 15.0) {
        robotController->Forward(Speed::HALF);
        while(robotController->GetUSDistanceAux() > 8.0);
        robotController->Forward(Speed::NONE);

        robotController->Grab(true); // IR sample detection
        delay(300);
        
        robotController->Forward(Speed::HALF);
        if(robotController->GetIRSample() == Infrared::WHITE) {
          robotController->Grab(false);
          delay(500);
          robotController->Grab(true);

          doing = Procedure::RETURNING_LAB;
        } else {
          robotController->Grab(false);
          doing = Procedure::SWEEP;
        }

        robotController->Forward(Speed::NONE);
        return;
      }
    }

    robotController->Turn(180);
    while(robotController->IsPerforming(Action::TURNING_RIGHT));
    robotController->Forward(Speed::NONE);

    doing = Procedure::FINDING_MOUNTAIN;
    return;
  } else if(doing == Procedure::FINDING_MOUNTAIN) {
    robotController->ToggleUSTurn(true);
    robotController->Forward(Speed::FULL);

    bool reachedBorder = false;

    while(!reachedBorder) {
      if(robotController->GetIRLeft() == Infrared::BLACK || robotController->GetIRRight() == Infrared::BLACK) {
        if(!avoid()) {
          reachedBorder = true;
        }
      } else if(robotController->GetUSDistance() < 25.0) {
        robotController->Forward(Speed::NONE);
        robotController->Turn(robotController->GetUSAngle()); // Turn towards the mountain
        robotController->ToggleUSTurn(false);
        while(robotController->IsPerforming(Action::TURNING_LEFT) || robotController->IsPerforming(Action::TURNING_RIGHT));

        bool foundPassage = false;
        bool left = true;
        short count = 0;
        double distance = 0.0;
        robotController->ResetTravelDist();

        robotController->Forward(Speed::FULL);
        robotController->Turn(-90);
        while(robotController->IsPerforming(Action::TURNING_LEFT));

        robotController->Forward(Speed::NONE);
        delay(200); // Give the sensor some time to acquire data
        if(robotController->GetUSDistance() < 25.0) {
          left = false;
          robotController->Turn(180);
          while(robotController->IsPerforming(Action::TURNING_RIGHT));
        }
        robotController->Forward(Speed::FULL);

        unsigned long startTime = millis();

        while(count < 3 && (startTime - millis()) < (20 * 1000)) {  // Timeout of 20s
          unsigned long startDriveTime = millis();
          while((startDriveTime - millis()) < 500) {
            if(robotController->GetIRLeft() == Infrared::BLACK || robotController->GetIRRight() == Infrared::BLACK) {
              if(!avoid()) {
                robotController->Turn(180);
                while(robotController->IsPerforming(Action::TURNING_RIGHT));
                
                robotController->Forward(Speed::NONE);
                doing == Procedure::SWEEP;
                return;
              }
            }
          }
          
          robotController->Turn(left ? 90 : -90); // Turn back to look at the mountain
          while(left ? robotController->IsPerforming(Action::TURNING_RIGHT) : robotController->IsPerforming(Action::TURNING_LEFT));

          robotController->Forward(Speed::NONE);
          delay(200);
          if(robotController->GetUSDistance() > 25.0) {
            foundPassage = true;
            break;
          }
          robotController->Forward(Speed::FULL);

          robotController->Turn(left ? -90 : 90);
          while(left ? robotController->IsPerforming(Action::TURNING_LEFT) : robotController->IsPerforming(Action::TURNING_RIGHT));

          count++;
        }

        if(foundPassage) {         
          distance = robotController->GetTravelDist();
          robotController->SetUSAngle(left ? 90 : -90);
          delay(1500); // Allow some time for the US turning and updating

          unsigned long startDriveTime = millis();
          
          while(robotController->GetUSDistance() < 25.0 && (millis() - startDriveTime) < (10 * 1000)) {
            if(robotController->GetIRLeft() == Infrared::BLACK || robotController->GetIRRight() == Infrared::BLACK) {
              if(!avoid()) {
                robotController->Turn(180);
                while(robotController->IsPerforming(Action::TURNING_RIGHT));
                
                robotController->Forward(Speed::NONE);
                doing == Procedure::SWEEP;
                return;
              }
            }
          }
          
          robotController->SetUSAngle(0);

          robotController->Turn(left ? 90 : -90);
          while(left ? robotController->IsPerforming(Action::TURNING_RIGHT) : robotController->IsPerforming(Action::TURNING_LEFT));

          startDriveTime = millis();
          robotController->ResetTravelDist();
          while(robotController->GetTravelDist() < distance && (millis() - startDriveTime) < (10 * 1000)) {
            if(robotController->GetIRLeft() == Infrared::BLACK || robotController->GetIRRight() == Infrared::BLACK) {
              if(!avoid()) {
                robotController->Turn(180);
                while(robotController->IsPerforming(Action::TURNING_RIGHT));
                
                robotController->Forward(Speed::NONE);
                doing == Procedure::SWEEP;
                return;
              }
            }
          }
        }

        robotController->Forward(Speed::NONE);
        doing = Procedure::SWEEP;
        return;
      }
    }

    robotController->Turn(180);
    while(robotController->IsPerforming(Action::TURNING_RIGHT));
    robotController->Forward(Speed::NONE);

    doing = Procedure::SWEEP;
    return;
  } else if(doing == Procedure::RETURNING_LAB) {
    robotController->ToggleUSTurn(true);
    
    short count = 0;
    bool foundLab = false;
    unsigned long startSearchTime;

    while(count < 10 && !foundLab) {
      robotController->Forward(Speed::FULL);
      robotController->Turn(180);
      while(robotController->IsPerforming(Action::TURNING_RIGHT));
      robotController->Forward(Speed::NONE);

      startSearchTime = millis();
      
      while((millis() - startSearchTime) < (25 * 1000)) {
        if(robotController->GetIRLab() == Infrared::WHITE) {
          robotController->Forward(Speed::FULL);
          robotController->Turn(robotController->GetUSAngle()); // Turn towards the lab
          robotController->ToggleUSTurn(false);
          while(robotController->IsPerforming(Action::TURNING_LEFT) || robotController->IsPerforming(Action::TURNING_RIGHT));
          robotController->Forward(Speed::NONE);

          foundLab = true;
          break;          
        }
      }
    }

    if(foundLab) {
      // todo
    }
    
    robotController->Forward(Speed::NONE);
    doing = Procedure::SWEEP;
  }


#elif ALGORITHM == 4 /* Collector mode */

  robotController->Forward(Speed::FULL);

#endif
}

#if ALGORITHM == 3
  bool avoid() {
    bool left = (robotController->GetIRLeft() == Infrared::BLACK);
    short count = 0;
    bool foundPassage = false;
    double distance = 0.0;

    robotController->Reverse(Speed::FULL);
    delay(500);
    robotController->Forward(Speed::FULL);

    while(count < 3 && !foundPassage) {
      robotController->Turn(left ? -90 : 90);
      while(robotController->IsPerforming(Action::TURNING_LEFT) || robotController->IsPerforming(Action::TURNING_RIGHT));

      robotController->ResetTravelDist();
      delay(500);
      distance += robotController->GetTravelDist();

      robotController->Turn(left ? 90 : -90); // turn back to look at line/cliff
      while(robotController->IsPerforming(Action::TURNING_LEFT) || robotController->IsPerforming(Action::TURNING_RIGHT));

      foundPassage = true;

      unsigned long startDriveTime = millis();

      while((startDriveTime - millis()) < 1000) {
        if(robotController->GetIRLeft() == Infrared::BLACK || robotController->GetIRRight() == Infrared::BLACK) {
          foundPassage = false;
        }
      }
    }
    
    if(foundPassage) {
      robotController->ResetTravelDist();
      robotController->Turn(left ? 90 : -90);
      while(robotController->IsPerforming(Action::TURNING_LEFT) || robotController->IsPerforming(Action::TURNING_RIGHT));

      unsigned long startDriveTime = millis();     
      while(robotController->GetTravelDist() < distance && (millis() - startDriveTime) < (10 * 1000));

      robotController->Turn(left ? -90 : 90);
      while(robotController->IsPerforming(Action::TURNING_LEFT) || robotController->IsPerforming(Action::TURNING_RIGHT));
    }

    return foundPassage;
  }
#endif

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
