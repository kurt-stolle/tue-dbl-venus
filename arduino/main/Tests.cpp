#include "Tests.h"

static void Tests::Run(RobotController* ctrl){
  ctrl->Forward(Speed::FULL);
  delay(1000);
  ctrl->Forward(Speed::HALF);
  delay(1000);
  ctrl->Turn(180);
  ctrl->Grab();
}

