#ifndef ScoutAlgorithm_h
#define ScoutAlgorithm_h

/*

  ALGORITHM DESCRIPTION:

  This algo will make the robot scout for samples to pick up and bring to the lab

*/

#include "Algorithm.h"
#include "RobotController.h"

namespace Scout {
enum Procedure {
  SWEEP,
  FINDING_SAMPLE,
  FINDING_MOUNTAIN,
  RETURNING_LAB
};
};

class ScoutAlgorithm : public Algorithm<Scout::Procedure> {
  public:
    void setup(RobotController* c); // Setup function
    void loop(RobotController* c); // Loop function
  private:
};

#endif
