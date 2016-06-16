#include "Arduino.h"
#include "RobotController.h"
#include "Layout.h"

/*
   Unit conversion
*/
#define SERVO_MIN 750
#define SERVO_MAX 2250

#define TURN_DELTA_BOUNDS 0.01

inline double degToMs(double deg) {
  // Thank you, Google.
  if (deg < -90) deg = -90;
  if (deg > 90) deg = 90;

  return map(deg, -90, 90, SERVO_MIN, SERVO_MAX);
}

inline double msToDeg(double ms) {
  if (ms < SERVO_MIN) ms = SERVO_MIN;
  if (ms > SERVO_MAX) ms = SERVO_MAX;

  return map(ms, SERVO_MIN, SERVO_MAX, -90, 90);
}

/*
   Constructor
*/
RobotController::RobotController() {
  this->usSensorServo.attach(PIN_SERVO_ULTRASOUND);
  this->usSensorServo.write(degToMs(0));
  this->lastUSTurn = millis();
  this->usSensorMain.Attach(PIN_TRIGGER_ULTRASOUND, PIN_ECHO_ULTRASOUND);
  this->usSensorAux.Attach(PIN_TRIGGER_ULTRASOUNDAUX, PIN_ECHO_ULTRASOUNDAUX);
  this->irSensorLeft.Attach(PIN_IR_LEFT, false);
  this->irSensorRight.Attach(PIN_IR_RIGHT, false);
  this->irLab.Attach(PIN_IR_LAB, true);
  this->irSample.Attach(PIN_IR_SAMPLE, true);
  this->servoGrabber.attach(PIN_SERVO_GRABBER);

  this->lastMovementUpdate = micros();
  this->distanceTraveled = 0.0;
  this->turnTarget = 0.0;
  this->state = Action::NONE;
  this->usDistance = DISTANCE_INFINITE;
  this->usDistanceAux = DISTANCE_INFINITE;
  this->usTurnEnabled = false;
  this->usAngle = 0.0;

  pinMode(PIN_RIGHT_ENCODER, INPUT);

  this->RPM = 0;
  this->RPMLastEncoderValue = 0;
  this->RPMLastEncoderEdge = 0.0;
  for (char a = 0; a < WHEEL_AVERAGE; a++) {
    this->RPMRunningAverage[a] = 0.0;
  }
}

/*
   Helper methods
*/
void RobotController::Forward(int speed) {
  // Stop
  if (speed == Speed::NONE) {
    this->removeAction(Action::MOVING_BACKWARD);
    this->removeAction(Action::MOVING_FORWARD);
  }
  // Forward
  else if (speed > Speed::NONE) {
    this->removeAction(Action::MOVING_BACKWARD);
    this->addAction(Action::MOVING_FORWARD);
  }
  // Backward
  else {
    this->removeAction(Action::MOVING_FORWARD);
    this->addAction(Action::MOVING_BACKWARD);
  }

  this->movementSpeed = speed;
}

void RobotController::Reverse(int speed) {
  this->Forward(-speed);
}

// Turning
void RobotController::Turn(double deg) {
  if (deg > 0.0) {
    this->addAction(Action::TURNING);
    this->addAction(Action::TURNING_RIGHT);
    this->removeAction(Action::TURNING_LEFT);
  } else if (deg < 0.0) {
    this->addAction(Action::TURNING);
    this->addAction(Action::TURNING_LEFT);
    this->removeAction(Action::TURNING_RIGHT);
  }

  this->turnTarget = deg;
}

// Grab an object
void RobotController::Grab(bool grab) {
  this->servoGrabber.write(grab ? degToMs(-50) : degToMs(0));
}

// Scan using the US sensor
void RobotController::Scan() {
  if (this->IsPerforming(Action::SCANNING)) {
    this->usDistance = DISTANCE_INFINITE;
  } else {
    this->addAction(Action::SCANNING);
  }

  if (this->IsPerforming(Action::SCANNINGAUX)) {
    this->usDistanceAux = DISTANCE_INFINITE;
  } else {
    this->addAction(Action::SCANNINGAUX);
  }

  this->usSensorMain.SendPulse();
  this->usSensorAux.SendPulse();
}

// UltraSonic listen
void RobotController::USListen() {
  double distance = this->usSensorMain.GetDistance();

  if (distance != -1) {
    this->usDistance = distance;
    this->removeAction(Action::SCANNING);
  }
}

void RobotController::USListenAux() {
  double distance = this->usSensorAux.GetDistance();

  if (distance != -1) {
    this->usDistanceAux = distance;
    this->removeAction(Action::SCANNINGAUX);
  }
}

double RobotController::GetUSAngle() {
  return msToDeg(this->usSensorServo.read());
}

void RobotController::SetUSAngle(double angle) {
  this->usAngle = -1 * angle;
}

double RobotController::GetUSDistance() {
  return this->usDistance;
}

double RobotController::GetUSDistanceAux() {
  return this->usDistanceAux;
}

void RobotController::ToggleUSTurn(bool enable) {
  this->usTurnEnabled = enable;
}

Infrared::Color RobotController::GetIRLeft() {
  return this->irSensorLeft.GetColor();
}

Infrared::Color RobotController::GetIRRight() {
  return this->irSensorRight.GetColor();
}

Infrared::Color RobotController::GetIRSample() {
  return this->irSample.GetColor();
}

Infrared::Color RobotController::GetIRLab() {
  return this->irLab.GetColor();
}

// State flag control
bool RobotController::IsPerforming(Action::Action a) {
  return (this->state & a);
}

void RobotController::addAction(Action::Action a) {
  this->state = this->state | a;
}

void RobotController::removeAction(Action::Action a) {
  this->state = this->state & (~a);
}

double RobotController::GetTravelDist() {
  return this->distanceTraveled;
}

void RobotController::ResetTravelDist() {
  this->distanceTraveled = 0.0;
}

// Update movement thread
inline void updateRPM(double* rpm, unsigned long* edge, uint8_t* last, double* arr, uint8_t val) {
  if (val != *last) {
    if (val == HIGH and * last == LOW) { // We're at a rising edge
      double rpmCur  = (double) (60000.0 / ((millis() - *edge) * ((double) CALIBRATION_WHEEL_HOLES)));

      /* Set bounds */
      if (rpmCur > *rpm + 8.0){
        rpmCur = *rpm + 8.0;
      }
      else if (rpmCur < *rpm - 8.0){
        rpmCur = *rpm - 8.0;
      }

      /* Average */
      *rpm = 0.0;
      for (char a = 0; a < WHEEL_AVERAGE; a++) {
        if (a == WHEEL_AVERAGE - 1) {
          arr[a] = rpmCur;
        } else {
          arr[a] = arr[a + 1];
        }

        *rpm += (arr[a] / WHEEL_AVERAGE);

      }
      *edge = millis();
    }
    *last = val;
  }

  if ((millis() - *edge) > 5000) {
    *rpm = 0;
  }
}

inline double wheelDistanceDelta(unsigned long dt, double rpm) {
  return ((double) (2 * PI * WHEEL_RADIUS)) * ((double)(rpm * (dt / 60000.0))); // dt is in millis, rpm is in min^-1
}

void RobotController::UpdateMovement() {
  /*
     Time
  */
  unsigned long dt = millis() - this->lastMovementUpdate; // Time delta in microseconds
  this->lastMovementUpdate = millis();

  if (dt > 150) return; // time-out

  /*
     RPM calculations
  */
  updateRPM(&this->RPM, &this->RPMLastEncoderEdge, &this->RPMLastEncoderValue, this->RPMRunningAverage, digitalRead(PIN_RIGHT_ENCODER));

  /*
     Ultrasonic movement
  */
  if (!this->usTurnEnabled) {
    this->usSensorServo.write(degToMs(this->usAngle));
  } else if ((millis() - this->lastUSTurn) > CALIBRATION_TIME_US_TURN) {
    double newPosition = msToDeg(this->usSensorServo.read()) + 30.0;
    
    if (newPosition > 90.0) {
      newPosition = -90.0;
      this->lastUSTurn = millis(); // allow some extra time to turn back
    } else {
      this->lastUSTurn = millis();
    }
    Serial.println(newPosition);
    this->usSensorServo.write(degToMs(newPosition));
  }

  /*
     Wheel movement
  */
  int speed;
  if (this->IsPerforming(Action::MOVING_FORWARD) || this->IsPerforming(Action::MOVING_BACKWARD)) {
    speed = this->movementSpeed;
  } else {
    speed = Speed::NONE;
  }



  // Calculate the delta for how far we have turned (and moved) since last time
  double dx = wheelDistanceDelta(dt, this->RPM);
  double modLeft;
  double modRight;

  // Apply a modulation to the wheel speed
  if (this->turnTarget > 2.0) {
    // Turn left
    this->turnTarget -= (dx / (WHEEL_DISTANCE_APART / 2.0)) * (180.00 / PI);

    if (this->turnTarget < 0.0){ // Cut off overshoot. Servo not accurate enough.
      this->turnTarget = 0.0;
      modLeft = 1;
      modRight = 1;
    } else {
      modLeft = 1;
      modRight = -1;
    }
  } else if (this->turnTarget < -2.0) {
    // Turn right
    this->turnTarget += (dx / (WHEEL_DISTANCE_APART / 2.0)) * (180.00 / PI);
    if (this->turnTarget > 0.0){ 
      this->turnTarget = 0.0;
      modLeft = 1;
      modRight = 1;
    } else {
      modLeft = -1;
      modRight = 1;
    }
  } else {
    this->distanceTraveled += dx;

    // Do not turn - no modulation needed
    this->removeAction(Action::TURNING);
    this->removeAction(Action::TURNING_LEFT);
    this->removeAction(Action::TURNING_RIGHT);

    this->turnTarget = 0;
    modLeft = 1;
    modRight = 1;
  }

  // Apply our calculations
  double leftSpeed = speed * modLeft;
  double rightSpeed = speed * modRight;

  if (abs(leftSpeed) < 1) {
    this->wheelLeft.detach();
  } else {
    if (!this->wheelLeft.attached()) {
      this->wheelLeft.attach(PIN_MOTOR_LEFT);
    }

    this->wheelLeft.write(degToMs(leftSpeed));
  }
  if (abs(rightSpeed) < 1) {
    this->wheelRight.detach();
  } else {
    if (!this->wheelRight.attached()) {
      this->wheelRight.attach(PIN_MOTOR_RIGHT);
    }

    this->wheelRight.write(degToMs(-rightSpeed));
  }
}
