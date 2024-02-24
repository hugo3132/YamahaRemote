#pragma once
#include <TFT_eSPI.h>

class ViewBase {
protected:
  TFT_eSPI& tft;

public:
  ViewBase(TFT_eSPI& tft)
  : tft(tft) {}

  virtual bool evaluateTouch(const uint16_t& x, const uint16_t& y) = 0;

};