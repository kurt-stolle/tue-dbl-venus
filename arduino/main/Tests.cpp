#include "Tests.h"
#include "Arduino.h"

static bool busy = false;

void Tests::Run(RobotController* ctrl){
  if (busy) return;
  busy = !busy;
    
  ctrl->Forward(Speed::FULL);
  delay(1000);
  ctrl->Forward(Speed::HALF);
  delay(1000);
  ctrl->Turn(180);
  ctrl->Grab();

  busy = !busy;
}

