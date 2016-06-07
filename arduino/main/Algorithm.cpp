#include "Algorithm.h"

/*
 * Implementation
 */
 #include "Algorithm.h"


template <class Procedure> void Algorithm<Procedure>::setProcedure(Procedure p) {
  this->procedure = p;
}

template <class Procedure> Procedure Algorithm<Procedure>::getProcedure() {
  return this->procedure;
}

/*
 * Define possiblle templates
 */

 #include "CalibrationAlgorithm.h"
 #include "ScoutAlgorithm.h"
 #include "CollectorAlgorithm.h"

 template class Algorithm<Scout::Procedure>;
 template class Algorithm<Calibration::Procedure>;
 template class Algorithm<Collector::Procedure>;
