/*
 *  TU/e Venus OGO
 *  Controller 
 *  Implementation
 */

#include "Controller.h"

/*
 * Constructor
 */
Controller::Controller(){
  // Nothing so far
}

/*
 * Public methods
 */
void Controller::Turn(double deg){
  
}

void Controller::Grab(){
  
}

void Controller::Scan(){
  
}

bool Controller::IsPerforming(action a) {
  return (this->state & a);
}

/*
 * Private methods
 */
void Controller::addAction(action a){
  this->state = this->state | a;
}

void Controller::removeAction(action a){
  this->state = this->state ^ a;
}

