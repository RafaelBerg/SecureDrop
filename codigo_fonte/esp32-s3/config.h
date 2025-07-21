#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

struct Config {
  String wifiSSID;
  String wifiPassword;
  String apSSID;
  String apPassword;
  String telegramBotToken;
  String telegramChatId;
  String codeBox;
};

extern Config deviceConfig;

bool loadConfig();
bool saveConfig();
void resetConfig();
void configDisplayLoop();

#endif