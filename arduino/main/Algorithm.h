#ifndef Algorithm_h
#define Algorithm_h

#include "Arduino.h"
#include "RobotController.h"

template <class Procedure> class Algorithm {
protected:
  void setProcedure(Procedure p);           // Set the current procedure
  Procedure getProcedure();                 // Get the current procedure
  RobotController* controller;
public:
  Algorithm(RobotController* c);
  virtual void loop();    // Must be overriden
  virtual void setup();   // Ditto.
private:
  Procedure procedure;
};

#endif
