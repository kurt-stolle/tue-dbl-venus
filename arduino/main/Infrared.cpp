#include "Infrared.h"

#define GRAY_THOLD 15
#define WHITE_THOLD 200

Infrared::Infrared(pin measurePin) {
  this->measurePin = measurePin;
}

Infrared::Color Infrared::GetColor() {
  // TODO

  // measure pin and return color (Infrared::WHITE, Infrared::GRAY, Infrared::BLACK)
  int measurement = 1;
  if (measurement < GRAY_THOLD) {
    return Infrared::BLACK;
  }

  if (measurement >= GRAY_THOLD && measurement < WHITE_THOLD) {
    return Infrared::GRAY;
  }

  if (measurement >= WHITE_THOLD) {
    return Infrared::WHITE;
  }
}


