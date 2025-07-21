#ifndef TELEGRAM_HELPER_H
#define TELEGRAM_HELPER_H

#include <WiFi.h>
#include <HTTPClient.h>

void sendTelegramMessage(const String& message);

#endif