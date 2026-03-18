#include <Arduino.h>

const int TRIG_PIN = 5;
const int ECHO_PIN = 18;
const int LED_VERDE = 21;
const int LED_GALBEN = 22;
const int LED_ROSU = 23;
const int BUZZER_PIN = 19;

volatile unsigned long pulseStartTime = 0;
volatile unsigned long pulseDuration = 0;
volatile bool newDistanceAvailable = false;

void IRAM_ATTR echoISR() {
  if (digitalRead(ECHO_PIN) == HIGH) {
    pulseStartTime = micros();
  } else {
    pulseDuration = micros() - pulseStartTime;
    newDistanceAvailable = true;
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(LED_VERDE, OUTPUT);
  pinMode(LED_GALBEN, OUTPUT);
  pinMode(LED_ROSU, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(ECHO_PIN), echoISR, CHANGE);
}

void updateVisuals(float distance) {
  digitalWrite(LED_VERDE, LOW);
  digitalWrite(LED_GALBEN, LOW);
  digitalWrite(LED_ROSU, LOW);
  digitalWrite(BUZZER_PIN, LOW);

  if (distance <= 0 || distance > 100) return;

  if (distance > 50) {
    digitalWrite(LED_VERDE, HIGH);
  } else if (distance > 15) {
    digitalWrite(LED_GALBEN, HIGH);
    digitalWrite(BUZZER_PIN, (millis() % 500 < 100));
  } else {
    digitalWrite(LED_ROSU, HIGH);
    digitalWrite(BUZZER_PIN, HIGH);
  }
}

void loop() {
  static unsigned long lastTrigger = 0;
  if (millis() - lastTrigger >= 100) {
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);
    lastTrigger = millis();
  }

  if (newDistanceAvailable) {
    float distanceCm = (pulseDuration * 0.0343) / 2;
    newDistanceAvailable = false;
    updateVisuals(distanceCm);
    Serial.printf("Distanta: %.2f cm\n", distanceCm);
  }
}