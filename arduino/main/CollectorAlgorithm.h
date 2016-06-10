#ifndef CollectorAlgorithm_h
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
  AVOIDING_MOUNTAIN,
  AVOIDING_CLIFF,
  GET_SAMPLE,
  RETURNING_LAB
};
};

class CollectorAlgorithm : public Algorithm<Collector::Procedure> {
  public:
    void setup(RobotController* c); // Setup function
    void loop(RobotController* c); // Loop function
   private:
   int whichSide;
};

#endif
