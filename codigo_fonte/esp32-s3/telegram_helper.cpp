#include "telegram_helper.h"
#include "config.h"

void sendTelegramMessage(const String& message) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi desconectado, não pode enviar mensagem");
    return;
  }

  HTTPClient http;
  String url = "https://api.telegram.org/bot" + deviceConfig.telegramBotToken + "/sendMessage?chat_id=" + deviceConfig.telegramChatId + "&text=" + message;

  http.begin(url);
  int httpCode = http.GET();
  if (httpCode > 0) {
    String payload = http.getString();
    Serial.println("Telegram resposta: " + payload);
  } else {
    Serial.println("Erro ao enviar mensagem Telegram");
  }
  http.end();
}