#include "hardware_helpers.h"
#include <Arduino.h>
#include <Wire.h>
#include "telegram_helper.h"
#include "code_storage.h"
#include "config.h"

static const int servoPin = 37;
static const int sensorPin = 38; 
static const int sdaPin = 20;
static const int sclPin = 21;

static bool waitingForSensor = false;
static unsigned long sensorWaitStart = 0;
static String lastCode = "";
static int lastIndex = 0;

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Servo myServo;

void initHardware() {
  Wire.begin(sdaPin, sclPin);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("Falha ao iniciar o display OLED"));
    while(true);
  }

  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);

  myServo.attach(servoPin);
  myServo.write(85); //FECHADO 
  /* myServo.write(0); */ // aberto 

  pinMode(sensorPin, INPUT_PULLUP);
}

void displayMessage(const char* message, int line, int textSize, bool clear) {
  if(clear){
    display.clearDisplay();
  }
  display.setTextSize(textSize);   
  display.setCursor(0, line * 16);
  display.println(message);
  display.display();
}

void openBox(String code) {
  if(code != ""){
    lastCode = code;
  }
  myServo.write(0);
  waitingForSensor = true;
  sensorWaitStart = 0;
  Serial.println("Caixa aberta!");
}

void deliverCode(String code, int index) {
  lastCode = code;
  lastIndex = index;
  displayMessage("Caixa", 0, 2, true);
  displayMessage("aberta! ", 1, 2);
  displayMessage("Entregue o", 2, 2);
  displayMessage("produto", 3, 2);
  openBox();
}

void checkSensorState() {
  if (!waitingForSensor) return;

  unsigned long now = millis();

  if (sensorWaitStart == 0) {
    sensorWaitStart = now;
  }

  if (now - sensorWaitStart >= 7000) {
    int sensorState = digitalRead(sensorPin);
    Serial.print("Sensor após 7s: ");
    Serial.println(sensorState == LOW ? "CONECTADO" : "DESCONECTADO");

    if (sensorState != LOW) {
      Serial.println("Caixa continua aberta!");
      while (digitalRead(sensorPin) != LOW) {
        delay(100);
      }
    }

    Serial.println("Caixa fechada!");
    
    myServo.write(85);
    waitingForSensor = false;
    sensorWaitStart = 0;
    if (!lastCode.equals(deviceConfig.codeBox)){
      displayMessage("Produto", 0, 2, true);
      displayMessage("entregue", 1, 2);
      displayMessage("com", 2, 2);
      displayMessage("sucesso!", 3, 2);
      sendTelegramMessage("Código " + lastCode + " foi entregue!");
      codes[lastIndex].status = "entregue";
      codes[lastIndex].date = getCurrentDateTime();
      Serial.println("Código atualizado para entregue.");
      saveCodes();  
      delay(5000);
    }
    displayMessage("Escaneie o QR CODE", 0, 2, true);
    displayMessage("para abrir a caixa", 2, 2);
  }
}