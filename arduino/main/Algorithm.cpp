#include "Algorithm.h"

template <class Procedure> void Algorithm<Procedure>::setProcedure(Procedure p){
    this->procedure = p;
}

template <class Procedure> Procedure Algorithm<Procedure>::getProcedure(){
  return this->procedure;
}
