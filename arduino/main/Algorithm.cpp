#include "Algorithm.h"

template <class Procedure> Algorithm<Procedure>::Algorithm(RobotController* c){
  this->controller = c;
}

template <class Procedure> void Algorithm<Procedure>::setProcedure(Procedure p){
    this->procedure = p;
}

template <class Procedure> Procedure Algorithm<Procedure>::getProcedure(){
  return this->procedure;
}
