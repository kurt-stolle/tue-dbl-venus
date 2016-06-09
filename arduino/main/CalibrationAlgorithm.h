#ifndef CalibrationAlgorithm_h
#define CalibrationAlgorithm_h

/*

  ALGORITHM DESCRIPTION:

  This algo will turn the wheels at zero speed in order to calibrate the servo's

*/

#include "Algorithm.h"
#include "RobotController.h"

namespace Calibration {
enum Procedure {
  CALIBRATING
};
};

class CalibrationAlgorithm : public Algorithm<Calibration::Procedure> {
  public:
    void setup(RobotController* c); // Setup function
    void loop(RobotController* c); // Loop function
};

#endif
