#define BLYNK_TEMPLATE_ID "TMPL2v6q_U6G1"
#define BLYNK_TEMPLATE_NAME "SYSTEM SECURITY"

#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <BlynkSimpleStream.h>

// ===== Auth Token =====
char auth[] = "UldGf8ePbc_l6qJb-_7Y2MwGXY842C6J";

// ===== BMP180 =====
Adafruit_BMP085 bmp;

// ===== Capteurs =====
#define GAS     A0
#define PIR     A1
#define FIRE    10
#define BUZZER  A2

int pirValue = 0;
int gasValue = 0;
int fireValue = 0;

int fireState = 0;
int pirState  = 0;
int gasState  = 0;

BlynkTimer timer;

// ==========================================
void sendSensorData() {

  // ===== BMP180 =====
  float temperatureBMP = bmp.readTemperature();
  float pression = bmp.readPressure() / 100.0;
  float altitude = bmp.readAltitude();

  // ===== Lecture capteurs =====
  fireValue = digitalRead(FIRE);
  pirValue  = analogRead(PIR);
  gasValue  = analogRead(GAS);

  // ===== Etats =====
  fireState = (fireValue == 1) ? 1 : 0;
  pirState  = (pirValue >= 650) ? 1 : 0;
  gasState  = (gasValue >= 620) ? 1 : 0;

  // ===== LEDs Blynk =====
  Blynk.virtualWrite(V0, fireState);
  Blynk.virtualWrite(V1, pirState);
  Blynk.virtualWrite(V2, gasState);

  // ===== BMP180 =====
  Blynk.virtualWrite(V3, temperatureBMP);
  Blynk.virtualWrite(V4, pression);
  Blynk.virtualWrite(V5, altitude);

  // ===== Buzzer =====
  if (fireState == 0 && pirState == 0 && gasState == 0) {
    noTone(BUZZER);
  }
  else if (fireState + pirState + gasState >= 2) {
    tone(BUZZER, 1200);
  }
  else {
    tone(BUZZER, 600);
  }
}

// ==========================================
void setup() {

  Serial.begin(9600);   // obligatoire pour Blynk Stream

  Blynk.begin(Serial, auth);

  if (!bmp.begin()) {
    while (1);
  }

  pinMode(FIRE, INPUT);
  pinMode(PIR, INPUT);
  pinMode(GAS, INPUT);
  pinMode(BUZZER, OUTPUT);

  timer.setInterval(2000L, sendSensorData);
}

// ==========================================
void loop() {
  Blynk.run();
  timer.run();
}