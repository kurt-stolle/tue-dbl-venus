#include "Infrared.h"
#include "Arduino.h"

Infrared::Infrared(pin measurePin) {
  this->measurePin = measurePin;
}

Infrared::Color Infrared::GetColor() {
  // Measure pin and return color (Infrared::WHITE, Infrared::GRAY, Infrared::BLACK)
  
  int measurement = analogRead(this->measurePin);
  
  if (measurement < GRAY_THOLD) {
    return Infrared::BLACK;
  } else if (measurement >= GRAY_THOLD && measurement < WHITE_THOLD) {
    return Infrared::GRAY;
  }
  
  return Infrared::WHITE;
}


