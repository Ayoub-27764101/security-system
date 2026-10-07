/*
 * ============================================================
 *  SYSTEM SECURITY - Systeme de securite connecte (Arduino UNO)
 * ============================================================
 *  Surveille trois dangers et envoie l'etat a l'application Blynk :
 *    - Flamme      (capteur de flamme, sortie numerique)
 *    - Mouvement   (capteur PIR, lu en analogique)
 *    - Gaz         (capteur de gaz, sortie analogique)
 *  Mesure aussi la temperature, la pression et l'altitude (BMP180).
 *  Un buzzer sonne quand au moins un danger est detecte.
 *
 *  Brochage :
 *    A0  -> capteur de gaz
 *    A1  -> capteur PIR
 *    A2  -> buzzer
 *    D10 -> capteur de flamme
 *    A4 (SDA) / A5 (SCL) -> BMP180 (bus I2C)
 *
 *  Widgets Blynk (broches virtuelles) :
 *    V0 = flamme   V1 = mouvement   V2 = gaz        (0 ou 1)
 *    V3 = temperature (degC)   V4 = pression (hPa)   V5 = altitude (m)
 *
 *  Bibliotheques : Blynk, Adafruit BMP085 Library (compatible BMP180)
 * ============================================================
 */

// Identifiants du modele Blynk : doivent etre definis AVANT d'inclure Blynk
#define BLYNK_TEMPLATE_ID "TMPL2v6q_U6G1"
#define BLYNK_TEMPLATE_NAME "SYSTEM SECURITY"

#include <Wire.h>               // bus I2C (utilise par le BMP180)
#include <Adafruit_BMP085.h>    // pilote BMP085/BMP180
#include <BlynkSimpleStream.h>  // Blynk via la liaison serie (pas de WiFi)

// ===== Auth Token =====
// Jeton qui relie cette carte a votre appareil dans Blynk.
// ATTENTION : ce jeton est un secret. Ne le publiez pas dans un depot public ;
// s'il a ete publie, regenerez-le dans la console Blynk.
char auth[] = "UldGf8ePbc_l6qJb-_7Y2MwGXY842C6J";

// ===== BMP180 =====
Adafruit_BMP085 bmp;

// ===== Capteurs =====
#define GAS     A0   // capteur de gaz (analogique)
#define PIR     A1   // capteur de mouvement PIR (lu en analogique)
#define FIRE    10   // capteur de flamme (numerique)
#define BUZZER  A2   // buzzer d'alarme

// Valeurs brutes lues sur les capteurs
int pirValue = 0;
int gasValue = 0;
int fireValue = 0;

// Etats apres comparaison aux seuils : 1 = danger detecte, 0 = normal
int fireState = 0;
int pirState  = 0;
int gasState  = 0;

// Minuterie Blynk : appelle sendSensorData() periodiquement sans bloquer loop()
BlynkTimer timer;

// ==========================================
// Lit tous les capteurs, envoie les valeurs a Blynk et pilote le buzzer.
// Appelee toutes les 2 secondes par la minuterie.
void sendSensorData() {

  // ===== BMP180 =====
  float temperatureBMP = bmp.readTemperature();
  float pression = bmp.readPressure() / 100.0;   // Pa -> hPa
  float altitude = bmp.readAltitude();           // en metres, pression de reference au niveau de la mer par defaut

  // ===== Lecture capteurs =====
  fireValue = digitalRead(FIRE);
  pirValue  = analogRead(PIR);
  gasValue  = analogRead(GAS);

  // ===== Etats =====
  // Flamme : le capteur donne directement 1 quand il detecte une flamme.
  // PIR et gaz : valeur analogique (0 a 1023) comparee a un seuil.
  // Ajustez 650 et 620 selon vos capteurs.
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
  // Aucun danger      -> silence
  // Un seul danger    -> son grave (600 Hz)
  // Deux dangers ou + -> son aigu (1200 Hz)
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
// Initialisation : executee une seule fois au demarrage.
void setup() {

  Serial.begin(9600);   // obligatoire pour Blynk Stream

  // Connexion a Blynk a travers le port serie
  Blynk.begin(Serial, auth);

  // Si le BMP180 ne repond pas (cablage I2C), on arrete le programme ici
  if (!bmp.begin()) {
    while (1);
  }

  pinMode(FIRE, INPUT);
  pinMode(PIR, INPUT);
  pinMode(GAS, INPUT);
  pinMode(BUZZER, OUTPUT);

  // Lecture et envoi des mesures toutes les 2000 ms
  timer.setInterval(2000L, sendSensorData);
}

// ==========================================
// Boucle principale : ne pas y mettre de delay(), sinon Blynk se deconnecte.
void loop() {
  Blynk.run();   // maintient la communication avec Blynk
  timer.run();   // declenche sendSensorData() quand c'est l'heure
}
