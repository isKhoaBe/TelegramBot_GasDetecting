#include "fsm_buttons.h"
#include "config.h"
#include "mq2_sensor.h"

ScreenState currentScreen = SCREEN_MONITOR;

unsigned long lastDebounceTime = 0;
const int debounceDelay = 200; 

void handleButtons() {
  if (millis() - lastDebounceTime < debounceDelay) return;

  if (digitalRead(BTN_MENU) == LOW) {
    lastDebounceTime = millis();
    lcd.clear();
    if (currentScreen == SCREEN_MONITOR) currentScreen = SCREEN_SET_WARN;
    else if (currentScreen == SCREEN_SET_WARN) currentScreen = SCREEN_SET_CRITICAL;
    else if (currentScreen == SCREEN_SET_CRITICAL) currentScreen = SCREEN_CALIBRATE;
    else if (currentScreen == SCREEN_CALIBRATE) currentScreen = SCREEN_MONITOR;
    while (digitalRead(BTN_MENU) == LOW) delay(10);
  }

  if (digitalRead(BTN_SAVE) == LOW) {
    lastDebounceTime = millis();
    if (currentScreen == SCREEN_MONITOR) return; 

    if (currentScreen == SCREEN_CALIBRATE) {
       calibrateSensor();
       currentScreen = SCREEN_MONITOR;
    } 
    else {
       lcd.clear(); lcd.setCursor(0,0); lcd.print("SAVING...");
       preferences.putInt("warn", warnThreshold);
       preferences.putInt("crit", criticalThreshold);
       delay(500);
       currentScreen = SCREEN_MONITOR;
    }
    lcd.clear();
    while(digitalRead(BTN_SAVE) == LOW) delay(10);
  }

  if (digitalRead(BTN_UP) == LOW) {
    lastDebounceTime = millis();
    if (currentScreen == SCREEN_SET_WARN) warnThreshold += 100;
    if (currentScreen == SCREEN_SET_CRITICAL) criticalThreshold += 100;
  }
  if (digitalRead(BTN_DOWN) == LOW) {
    lastDebounceTime = millis();
    if (currentScreen == SCREEN_SET_WARN) warnThreshold -= 100;
    if (currentScreen == SCREEN_SET_CRITICAL) criticalThreshold -= 100;
  }
}