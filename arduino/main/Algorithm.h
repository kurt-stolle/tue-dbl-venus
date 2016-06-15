#ifndef Algorithm_h
#define Algorithm_h

#include "Arduino.h"
#include "RobotController.h"

template <class Procedure> class Algorithm {
protected:
  void setProcedure(Procedure p);           // Set the current procedure
  Procedure getProcedure();                 // Get the current procedure
  bool avoid(RobotController* c);
  bool returnToLab(RobotController* c);
public:
  virtual void loop(RobotController* c);    // Must be overriden
  virtual void setup(RobotController* c);   // Ditto.
private:
  Procedure procedure;
};

#endif
