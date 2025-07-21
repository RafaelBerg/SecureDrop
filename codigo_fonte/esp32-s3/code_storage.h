#ifndef CODE_STORAGE_H
#define CODE_STORAGE_H

#include <Arduino.h>

#define MAX_CODES 10

struct CodeEntry {
  String code;
  String status;
  String date;
};

extern CodeEntry codes[MAX_CODES];
extern int codeCount;

void saveCodes();
void loadCodes();
String getCurrentDateTime();

#endif