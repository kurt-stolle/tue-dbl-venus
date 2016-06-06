#ifdef CollectorAlgorithm_h
#define CollectorAlgorithm_h

/*

ALGORITHM DESCRIPTION:

This algo will make the robot collect samples dropped by a scout robot

*/

#include "Algorithm.h"
#include "RobotController.h"

namespace Collector {
  enum Procedure {
    SWEEPING,
    RETURNING_LAB
  }
}

class CollectorAlgorithm : public Algorithm<Collector::Procedure> {
public:
  void setup(); // Setup function
  void loop(); // Loop function
}

#endif
