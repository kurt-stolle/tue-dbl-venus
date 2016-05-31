/*
*  TU/e Venus OGO
*  Infrared
*  Library for simple infrared functions
*/

#ifndef Infrared_h
#define Infrared_h

#include "Layout.h"

#define GRAY_THOLD 15
#define WHITE_THOLD 45

class Infrared {
  public:
    enum Color {
      WHITE = 1,
      GRAY = 2,
      BLACK = 0
    };

    void Attach(pin measurePin, bool longRange);
    Color GetColor();           // Returns measured color
  private:
    pin measurePin;
    int thresholdWhite;
    int thresholdGray;

    enum Threshold {
      THRESH_WHITE = 45,
      THRESH_GRAY = 15,
      THRESH_LR_WHITE = 100,
      THRESH_LR_GRAY = 100     
    };
};








#endif
