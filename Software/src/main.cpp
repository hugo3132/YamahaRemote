#include "Receiver.h"
#include "config.h"
#include "views/ViewMain.h"

#include <HTTPClient.h>
#include <SPI.h>
#include <TFT_eSPI.h> // Hardware-specific library
#include <WiFi.h>
TFT_eSPI tft; // Invoke custom library

#if TFT_WIDTH != 240 || TFT_HEIGHT != 320
  #error check the resolution is 320x240 in .pio\libdeps\az-delivery-devkit-v4\TFT_eSPI\TFT_Drivers\ILI9488_Defines.h
#endif

#define TFT_LED 22

Receiver receiver;
ViewBase* currentView;
ViewMain viewMain(tft, receiver);

void setup(void) {
  REG_WRITE(GPIO_OUT_W1TS_REG, BIT(GPIO_NUM_16)); // Guru Meditation Error Remediation set
  delay(1);
  REG_WRITE(GPIO_OUT_W1TC_REG, BIT(GPIO_NUM_16)); // Guru Meditation Error Remediation clear

  // Initialize Serial
  Serial.begin(115200);
  // wait for serial port to connect. Needed for native USB
  while (!Serial) {
  }
  Serial.setDebugOutput(true);
  delay(500);

  // Initialize display
  tft.init();
  tft.setRotation(3);
  tft.invertDisplay(1);
  uint16_t calData[5] = {194, 3505, 278, 3548, 7};
  tft.setTouch(calData);

  // Setup Coil output
  pinMode(TFT_LED, OUTPUT);
  digitalWrite(TFT_LED, 0);

  // Start connecting
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE);
  tft.drawCentreString("Connecting to ", 160, 10, 4);
  tft.drawCentreString(WIFI_SSID, 160, 60, 4);

  WiFi.disconnect();
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PWD);
  WiFi.setAutoReconnect(true);
  String dots = ".";
  tft.drawCentreString(dots, 160, 110, 4);

  // Play an animation while the connection is established
  int counter = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(2500);
    if (WiFi.status() == WL_CONNECTED) {
      break;
    }
    dots = dots + "*";
    if (dots.length() > 10) {
      dots = "  *";
    }
    tft.fillRect(0, 110, 320, 100, TFT_BLACK);
    tft.drawCentreString(dots, 160, 110, 4);
    WiFi.disconnect();
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PWD);
    WiFi.setAutoReconnect(true);
  }

  tft.fillRect(0, 110, 320, 100, TFT_BLACK);
  tft.drawCentreString("Connected", 160, 110, 4);

  receiver.pollInfo();
  viewMain.show(false);
  currentView = &viewMain;
  // analogMeter(); // Draw analogue meter
}

void loop() {
  uint16_t x = 0, y = 0; // To store the touch coordinates

  for (int i = 1; i < 10; i++) {
    // Pressed will be set true is there is a valid touch on the screen
    bool pressed = tft.getTouch(&x, &y);

    if (pressed) {
      if (currentView->evaluateTouch(x, y)) {
        receiver.pollInfo();
        viewMain.show(true);
        // wait until touch is released
        while (tft.getTouch(&x, &y)) {
          delay(10);
        }
        Serial.println("Done");
      }
    }
    delay(100);
  }
  // receiver.pollInfo();
  // viewMain.show(true);

  return;
}