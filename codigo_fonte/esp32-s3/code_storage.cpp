#include "code_storage.h"
#include <SPIFFS.h>
#include <time.h>

CodeEntry codes[MAX_CODES];
int codeCount = 0;

String getCurrentDateTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    return "0000-00-00 00:00";
  }
  char buffer[20];
  strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M", &timeinfo);
  return String(buffer);
}

void saveCodes() {
  File file = SPIFFS.open("/codes.json", FILE_WRITE);
  if (!file) {
    Serial.println("Falha ao abrir arquivo para escrita");
    return;
  }

  String json = "[";
  for (int i = 0; i < codeCount; i++) {
    json += "{\"code\":\"" + codes[i].code + "\",\"status\":\"" + codes[i].status + "\",\"date\":\"" + codes[i].date + "\"}";
    if (i < codeCount - 1) json += ",";
  }
  json += "]";
  file.print(json);
  file.close();
  Serial.println("Dados salvos no SPIFFS");
}

void loadCodes() {
  if (!SPIFFS.exists("/codes.json")) {
    Serial.println("Arquivo codes.json não existe");
    return;
  }

  File file = SPIFFS.open("/codes.json", FILE_READ);
  if (!file) {
    Serial.println("Falha ao abrir arquivo para leitura");
    return;
  }

  String json = file.readString();
  file.close();
  codeCount = 0;

  int start = 0;
  while (true) {
    int idxCode = json.indexOf("\"code\":\"", start);
    if (idxCode == -1) break;

    int idxCodeEnd = json.indexOf("\"", idxCode + 8);
    String code = json.substring(idxCode + 8, idxCodeEnd);

    int idxStatus = json.indexOf("\"status\":\"", idxCodeEnd);
    int idxStatusEnd = json.indexOf("\"", idxStatus + 10);
    String status = json.substring(idxStatus + 10, idxStatusEnd);

    int idxDate = json.indexOf("\"date\":\"", idxStatusEnd);
    int idxDateEnd = json.indexOf("\"", idxDate + 8);
    String date = json.substring(idxDate + 8, idxDateEnd);

    if (codeCount < MAX_CODES) {
      codes[codeCount] = {code, status, date};
      codeCount++;
    }

    start = idxDateEnd;
  }

  Serial.println("Dados carregados do SPIFFS");
}