#ifdef CalibrationAlgorithm_h
#define CalibrationAlgorithm_h

/*

ALGORITHM DESCRIPTION:

This algo will turn the wheels at full speed in order to calibrate the servo's

*/

#include "Algorithm.h"
#include "RobotController.h"

namespace Calibration {
  enum Procedure {
    PROC_CALIBRATING
  }
}

class CalibrationAlgorithm : public Algorithm<Calibration::Procedure> {
public:
  void setup(RobotController* c); // Setup function
  void loop(RobotController* c); // Loop function
}

#endif
