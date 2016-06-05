#ifdef ScoutAlgorithm_h
#define ScoutAlgorithm_h

/*

ALGORITHM DESCRIPTION:

This algo will make the robot scout for samples to pick up and bring to the lab

*/

#include "Algorithm.h"

namespace Scout {
  enum Procedure {
    SWEEPING,
    RETURNING_LAB
  }
}

class ScoutAlgorithm : public Algorithm<Scout::Procedure> {
public:
  using Algorithm<Scout::Procedure>::Algorithm;
  void setup(); // Setup function
  void loop(); // Loop function
}

#endif
