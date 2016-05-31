#include "Infrared.h"
#include "Arduino.h"

void Infrared::Attach(pin measurePin, bool longRange) {
  this->measurePin = measurePin;

  if(longRange) {
    this->thresholdWhite = THRESH_LR_WHITE;
    this->thresholdGray = THRESH_LR_GRAY;
  } else {
    this->thresholdWhite = THRESH_WHITE;
    this->thresholdGray = THRESH_GRAY;
  }
}

Infrared::Color Infrared::GetColor() {
  // Measure pin and return color (Infrared::WHITE, Infrared::GRAY, Infrared::BLACK)
  
  int measurement = analogRead(this->measurePin);
  
  if (measurement < this->thresholdWhite) {
    return BLACK;
  } else if (measurement >= this->thresholdGray && measurement < this->thresholdWhite) {
    return GRAY;
  }
  
  return WHITE;
}


