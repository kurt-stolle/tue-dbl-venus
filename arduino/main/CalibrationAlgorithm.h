#ifdef CalibrationAlgorithm_h
#define CalibrationAlgorithm_h

/*

ALGORITHM DESCRIPTION:

This algo will turn the wheels at full speed in order to calibrate the servo's

*/

#include "Algorithm.h"

namespace Calibration {
  enum Procedure {
    PROC_CALIBRATING
  }
}

class CalibrationAlgorithm : public Algorithm<Calibration::Procedure> {
public:
  using Algorithm<Calibration::Procedure>::Algorithm; // Inherit constructor
  void setup(); // Setup function
  void loop(); // Loop function
}

#endif
