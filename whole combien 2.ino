#include <Arduino.h>
#include <Wire.h>
#include "MAX30100_PulseOximeter.h"
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include "ESP8266WebServer.h"
#include <FirebaseArduino.h>

#define REPORTING_PERIOD_MS 1000

#define FIREBASE_HOST "covid-92bd7-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "mzXwZxL7PmaISwy8GnxzCjwbJjOHkcGsqMMjzKxM"

#define WIFI_SSID "Mera wala net"
#define WIFI_PASSWORD "12345678"
#define lm35 A0

PulseOximeter pox;

unsigned long previousMillis = 0;
const long interval = 1000;
volatile boolean heartBeatDetected = false;

void onBeatDetected() {
  heartBeatDetected = true;
  Serial.println("Beat!");
}
void setup() {
  Serial.begin(115200);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

  if (Firebase.failed()) {
    Serial.print("setting /message failed:");
    Serial.println(Firebase.error());
  }

  if (!pox.begin()) {
    Serial.println("failed");
    for (;;)
      ;
  } else {
    Serial.println("success");
  }

  pox.setOnBeatDetectedCallback(onBeatDetected);
}

// void temps(){
//   int celsius= analogRead(lm35)*0.48828125;

//     Serial.print(" temp");
//   Serial.println(celsius);
//     Firebase.setFloat("users/temp", celsius);
//     delay(1000);

// return;
// }
void pulseoxi() {
  float BPM = pox.getHeartRate();
  float SpO2 = pox.getSpO2();

  if (heartBeatDetected && BPM != 0) {

    if (SpO2 > 0) {
      Firebase.setFloat("users/bp", BPM);
      Firebase.setFloat("users/spO2", SpO2);
      Serial.print("bpm");
      Serial.println(BPM);
      Serial.print("SpO2");
      Serial.println(SpO2);
    }
  }
}

void loop() {


  pox.update();

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    pox.shutdown();

    pulseoxi();

    pox.resume();
    previousMillis = currentMillis;
  }
  //  temps();
}