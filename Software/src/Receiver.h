#pragma once
#include <Arduino.h>
#include <WiFiClient.h>

class Receiver {
public:
  bool mainPower = false;
  bool zone2Power = false;
  float zone2Volume = -30.0;
  String zone2Input = "";

  /**
   * Send a message to the amplifier
   */
  String post(String body) {
    const uint16_t port = 80;

    Serial.println(body);

    WiFiClient client;

    if (client.connect("192.168.178.40", port)) {
      client.println("POST /YamahaRemoteControl/ctrl HTTP/1.1");
      client.println("Connection: close");
      client.print("Content-Length: ");
      client.println(body.length());
      client.println();
      client.println(body);
    }
    else {
      return "";
    }

    int loops = 0;
    while (!client.available() && loops < 1000) {
      loops++;
      delay(1);
    }

    if (client.available() > 0) {
      String response = client.readString();
      client.stop();
      Serial.println(response);
      return response;
    }
    else {
      client.stop();
      return "";
    }
  }

  void toggleMainPower() {
    if (mainPower) {
      post("<YAMAHA_AV cmd=\"PUT\"><Main_Zone><Power_Control><Power>Standby</Power></Power_Control></Main_Zone></YAMAHA_AV>");
    }
    else {
      post("<YAMAHA_AV cmd=\"PUT\"><Main_Zone><Power_Control><Power>On</Power></Power_Control></Main_Zone></YAMAHA_AV>");
    }
  }

  void toggleZone2Power() {
    if (zone2Power) {
      post("<YAMAHA_AV cmd=\"PUT\"><Zone_2><Power_Control><Power>Standby</Power></Power_Control></Zone_2></YAMAHA_AV>");
    }
    else {
      post("<YAMAHA_AV cmd=\"PUT\"><Zone_2><Power_Control><Power>On</Power></Power_Control></Zone_2></YAMAHA_AV>");
    }
  }

  void setZone2Volume(float newVal) {
    int val = (int)((newVal+100) * 10 + 0.5) - 1000;
    Serial.println(val);
    post("<YAMAHA_AV cmd=\"PUT\"><Zone_2><Volume><Lvl><Val>" + String(val) +
         "</Val><Exp>1</Exp><Unit>dB</Unit></Lvl></Volume></Zone_2></YAMAHA_AV>");
  }

  void pollInfo() {
    int i1, i2;
    // poll main Zone
    String answer = post("<YAMAHA_AV cmd=\"GET\"><Main_Zone><Basic_Status>GetParam</Basic_Status></Main_Zone></YAMAHA_AV>");
    i1 = answer.indexOf("<Power>");
    i2 = answer.indexOf("</Power>", i1);
    if (i1 != -1 && i2 != -1) {
      mainPower = answer.substring(i1 + 7, i2) == "On";
    }

    answer = post("<YAMAHA_AV cmd=\"GET\"><Zone_2><Basic_Status>GetParam</Basic_Status></Zone_2></YAMAHA_AV>");
    i1 = answer.indexOf("<Power>");
    i2 = answer.indexOf("</Power>", i1);
    if (i1 != -1 && i2 != -1) {
      zone2Power = answer.substring(i1 + 7, i2) == "On";
    }

    i1 = answer.indexOf("<Volume><Lvl><Val>");
    i2 = answer.indexOf("</Val>", i1);
    if (i1 != -1 && i2 != -1) {
      zone2Volume = atof(answer.substring(i1 + 18, i2).c_str()) / 10.0;
    }
    i1 = answer.indexOf("<Input><Input_Sel>");
    i2 = answer.indexOf("</Input_Sel>", i1);
    if (i1 != -1 && i2 != -1) {
      zone2Input = answer.substring(i1 + 18, i2);
    }
  }
};