#ifndef HARDWARE_HELPERS_H
#define HARDWARE_HELPERS_H

#include <Adafruit_SSD1306.h>
#include <ESP32Servo.h>

extern Adafruit_SSD1306 display;
extern Servo myServo;

void initHardware();
void displayMessage(const char* message, int line = 0, int textSize = 2, bool clear = false);
void openBox(String code = "");
void deliverCode(String code, int index);
void checkSensorState();

#endif