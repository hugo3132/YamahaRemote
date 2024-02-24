#pragma once
#include <Arduino.h>
#include "../Receiver.h"
#include "ViewBase.h"

class ViewMain: public ViewBase {
Receiver& receiver;
public:
  ViewMain(TFT_eSPI& tft, Receiver& receiver)
    : ViewBase(tft)
    , receiver(receiver) {}

public:
  void show(bool refresh) {
    static String lastInput = "";
    static float lastVolume = 100000;
    static bool lastPowerMain = false;
    static bool lastPowerZone2 = false;

    if(!refresh) {
      tft.fillScreen(TFT_BLACK);
      tft.drawLine(100, 0, 100, 240, TFT_WHITE);
      tft.drawLine(100, 45, 320, 45, TFT_WHITE);
      
      tft.setTextColor(TFT_WHITE);
      tft.drawCentreString("Zone:", 50, 10, 4);

      volumeButton("- 1 dB", 110, 90);
      volumeButton("+ 1 dB", 220, 90);
      volumeButton("- 5 dB", 110, 165);
      volumeButton("+ 5 dB", 220, 165);

      onOffButton("1", receiver.mainPower, 50);
      onOffButton("2", receiver.zone2Power, 145);
      lastPowerMain = receiver.mainPower;
      lastPowerZone2 = receiver.zone2Power;

      lastInput = "";
      lastVolume = 100000;
    }
    
    tft.setTextColor(TFT_WHITE);
    if(lastInput != receiver.zone2Input) {
      tft.fillRect(110, 10, 210, 25, TFT_BLACK);
      tft.drawString("Input: " + receiver.zone2Input, 110, 10, 4);
      lastInput = receiver.zone2Input;      
    }

    if(lastVolume != receiver.zone2Volume) {
      tft.fillRect(110, 60, 210, 25, TFT_BLACK);
      tft.drawString("Volume: " + String(receiver.zone2Volume, 1) + " dB", 110, 60, 4);
      lastVolume = receiver.zone2Volume;      
    }

    if(lastPowerMain != receiver.mainPower) {
      onOffButton("1", receiver.mainPower, 50);
      lastPowerMain = receiver.mainPower;      
    }

    if(lastPowerZone2 != receiver.zone2Power) {
      onOffButton("2", receiver.zone2Power, 145);
      lastPowerZone2 = receiver.zone2Power;      
    }
  }

  void onOffButton(const char* label, const bool& state, const uint32_t& y){
    int32_t color;
    int32_t borderColor;
    if(state) {
      borderColor = TFT_GREEN;
      color = TFT_DARKGREEN;
    }
    else  {
      borderColor = TFT_RED;
      color = TFT_MAROON;
    }
    tft.fillRoundRect(10, y, 80, 85, 5, color);
    tft.drawRoundRect(10, y, 80, 85, 5, borderColor);

    tft.drawCentreString(label, 50, y+25, 6);
  }

  void volumeButton(const char* label, const uint32_t& x, const uint32_t& y){
    int32_t color = TFT_DARKGREY;
    int32_t borderColor = TFT_LIGHTGREY;

    tft.fillRoundRect(x, y, 100, 65, 5, color);
    tft.drawRoundRect(x, y, 100, 65, 5, borderColor);

    tft.drawCentreString(label, x+50, y+22, 4);
  }

  virtual bool evaluateTouch(const uint16_t& x, const uint16_t& y) override {
    if(x > 10 && x < 90) {
      // on off button
      if(y > 50 && y < 135) {
        receiver.toggleMainPower();
        return true;
      }
      else if(y > 145 && y < 230) {
        receiver.toggleZone2Power();
        return true;
      }
    }
    else if(x > 110 && x < 210) {
      // - Button
      if(y > 90 && y < 155) {
        receiver.setZone2Volume(receiver.zone2Volume - 1);
        return true;
      }
      else if(y > 165 && y < 230) {
        receiver.setZone2Volume(receiver.zone2Volume - 5);
        return true;
      }
    }
    else if(x > 220 && x < 320) {
      // + Button
      if(y > 90 && y < 155) {
        receiver.setZone2Volume(receiver.zone2Volume + 1);
        return true;
      }
      else if(y > 165 && y < 230) {
        receiver.setZone2Volume(receiver.zone2Volume + 5);
        return true;
      }
    }
    return false;

  }


};