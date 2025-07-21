#include <WebServer.h>
#include <ArduinoJson.h>

#include "routes.h"
#include "dashboard_html.h"
#include "config_html.h"
#include "code_storage.h"
#include "hardware_helpers.h"
#include "config.h"

String receivedMessage = "";

void rootPage(WebServer& server) {
  server.send_P(200, "text/html", dashboardHTML);
}

void configPage(WebServer& server) {
  server.send_P(200, "text/html", configHTML);
}

void handleReceive(WebServer& server) {
  if (server.hasArg("message")) {
    receivedMessage = server.arg("message");

    if(receivedMessage == deviceConfig.codeBox && deviceConfig.codeBox != ""){
      displayMessage("Caixa", 0, 2, true);
      displayMessage("aberta!", 1, 2);
      return openBox(receivedMessage);
    }
    
    Serial.println("Mensagem recebida: " + receivedMessage);

    bool found = false;
    bool isRepetido = false;

    for (int i = 0; i < codeCount; i++) {
      if (codes[i].code == receivedMessage) {
        found = true;

        if (codes[i].status == "pendente") {
          deliverCode(receivedMessage, i);
        } else {
          isRepetido = true;
          server.send(200, "text/plain", "repetido: " + receivedMessage);
          displayMessage("Codigo ", 0, 2, true);
          displayMessage("repetido!", 1, 2);
          delay(2000);
          displayMessage("Escaneie o QR CODE", 0, 2, true);
          displayMessage("para abrir a caixa", 2, 2);
        }
        break;
      }
    }

    if (!found) {
      displayMessage("Codigo invalido!", 0, 2, true);
      server.send(404, "text/plain", "Código não cadastrado");
    }

  } else {
    server.send(400, "text/plain", "Mensagem não encontrada");
  }
}

void handleGetCodes(WebServer& server) {
  String json = "[";
  for (int i = 0; i < codeCount; i++) {
    json += "{\"code\":\"" + codes[i].code + "\",\"status\":\"" + codes[i].status + "\",\"date\":\"" + codes[i].date + "\"}";
    if (i < codeCount - 1) json += ",";
  }
  json += "]";
  server.send(200, "application/json", json);
}

void handleAddCode(WebServer& server) {
  if (!server.hasArg("plain")) {
    server.send(400, "text/plain", "Body missing");
    return;
  }

  String body = server.arg("plain");
  int codeIndex = body.indexOf("\"code\":\"");
  if (codeIndex == -1) {
    server.send(400, "text/plain", "JSON inválido");
    return;
  }

  String code = body.substring(codeIndex + 8, body.indexOf("\"", codeIndex + 8));
  code.trim();

  if (code.length() == 0) {
    server.send(400, "text/plain", "Código vazio");
    return;
  }

  for (int i = 0; i < codeCount; i++) {
    if (codes[i].code == code) {
      server.send(400, "text/plain", "Código já cadastrado");
      return;
    }
  }

  if (codeCount < MAX_CODES) {
    codes[codeCount] = {code, "pendente", getCurrentDateTime()};
    codeCount++;
    saveCodes();
    server.send(200, "text/plain", "Código adicionado");
  } else {
    server.send(400, "text/plain", "Armazenamento cheio");
  }
}

void handleDeleteCode(WebServer& server) {
  if (!server.hasArg("code")) {
    server.send(400, "text/plain", "Código ausente");
    return;
  }

  String code = server.arg("code");
  bool removed = false;
  for (int i = 0; i < codeCount; i++) {
    if (codes[i].code == code) {
      for (int j = i; j < codeCount - 1; j++) {
        codes[j] = codes[j + 1];
      }
      codeCount--;
      removed = true;
      saveCodes();
      break;
    }
  }

  server.send(removed ? 200 : 404, "text/plain", removed ? "Código removido" : "Código não encontrado");
}

void handleGetConfig(WebServer& server) {
  StaticJsonDocument<512> doc;

  doc["wifiSSID"] = deviceConfig.wifiSSID;
  doc["wifiPassword"] = deviceConfig.wifiPassword;
  doc["apSSID"] = deviceConfig.apSSID;
  doc["apPassword"] = deviceConfig.apPassword;
  doc["botToken"] = deviceConfig.telegramBotToken;
  doc["chatId"] = deviceConfig.telegramChatId;
  doc["codeBox"] = deviceConfig.codeBox;

  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);
}

void handleUpdateConfig(WebServer& server) {
  if (!server.hasArg("plain")) {
    server.send(400, "text/plain", "JSON ausente");
    return;
  }

  String body = server.arg("plain");

  StaticJsonDocument<512> doc;
  DeserializationError error = deserializeJson(doc, body);

  if (error) {
    server.send(400, "text/plain", "Erro ao interpretar JSON");
    return;
  }

  deviceConfig.wifiSSID         = doc["wifiSSID"]      | deviceConfig.wifiSSID;
  deviceConfig.wifiPassword     = doc["wifiPassword"]  | deviceConfig.wifiPassword;
  deviceConfig.apSSID           = doc["apSSID"]        | deviceConfig.apSSID;
  deviceConfig.apPassword       = doc["apPassword"]    | deviceConfig.apPassword;
  deviceConfig.telegramBotToken = doc["botToken"]      | deviceConfig.telegramBotToken;
  deviceConfig.telegramChatId   = doc["chatId"]        | deviceConfig.telegramChatId;
  deviceConfig.codeBox   = doc["codeBox"]        | deviceConfig.codeBox;

  if (saveConfig()) {
    server.send(200, "text/plain", "Salvo com sucesso");

  } else {
    server.send(500, "text/plain", "Falha ao salvar a configuração");
  }
}

void handleResetConfig(WebServer& server) {
  resetConfig();
}

void setupRoutes(WebServer& server) {
  server.on("/", [&]() { rootPage(server); });
  server.on("/config", [&]() { configPage(server); });
  server.on("/send", [&]() { handleReceive(server); });
  server.on("/codes", HTTP_GET, [&]() { handleGetCodes(server); });
  server.on("/codes", HTTP_POST, [&]() { handleAddCode(server); });
  server.on("/codes", HTTP_DELETE, [&]() { handleDeleteCode(server); });
  server.on("/get-config", HTTP_GET, [&]() { handleGetConfig(server); });
  server.on("/update-config", HTTP_POST, [&]() { handleUpdateConfig(server); });
  server.on("/reset-config", HTTP_POST, [&]() { handleResetConfig(server); });
}