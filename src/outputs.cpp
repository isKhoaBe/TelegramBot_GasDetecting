#include "outputs.h"
#include "config.h"

unsigned long lastBeepTime = 0;
bool buzzerState = false;

void turnOffAllOutputs() {
  digitalWrite(LED_GREEN, LOW); digitalWrite(LED_YELLOW, LOW);
  digitalWrite(LED_RED, LOW); digitalWrite(BUZZER, LOW);
  buzzerState = false;
}

void buzzer(int interval) {
  unsigned long currentMillis = millis();
  if (currentMillis - lastBeepTime >= interval) {
    lastBeepTime = currentMillis;
    buzzerState = !buzzerState;
    digitalWrite(BUZZER, buzzerState ? HIGH : LOW);
  }
}