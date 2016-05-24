/*
*  TU/e Venus OGO
*  Infrared
*  Library for simple infrared functions
*/

#ifndef Infrared_h
#define Infrared_h

#include "Layout.h"

#define GRAY_THOLD 10
#define WHITE_THOLD 50

class Infrared {
  public:
    enum Color {
      WHITE = 1,
      GRAY = 2,
      BLACK = 0
    };

    Infrared(pin measurePin);
    Color GetColor();           // Returns measured color
  private:
    pin measurePin;
};








#endif
