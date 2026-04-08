#include "ui_display.h"
#include "config.h"
#include "outputs.h"

void runMonitorMode() {
  lcd.setCursor(0, 0);
  lcd.print("PPM: "); lcd.print(gasPPM); lcd.print("     ");

  lcd.setCursor(0, 1);
  if (gasPPM < warnThreshold) {
    lcd.print("SAFE           ");
    turnOffAllOutputs(); digitalWrite(LED_GREEN, HIGH);
  } 
  else if (gasPPM >= warnThreshold && gasPPM < criticalThreshold) {
    lcd.print("WARNING!       ");
    digitalWrite(LED_GREEN, LOW); digitalWrite(LED_YELLOW, HIGH); digitalWrite(LED_RED, LOW);
    buzzer(1000);
  } 
  else {
    lcd.print("CRITICAL!!!    ");
    digitalWrite(LED_GREEN, LOW); digitalWrite(LED_YELLOW, LOW); digitalWrite(LED_RED, HIGH);
    buzzer(200);
  }
}

void displaySettingMode(String title, int value) {
  lcd.setCursor(0, 0); lcd.print(title);
  lcd.setCursor(0, 1); lcd.print("Val: "); lcd.print(value); lcd.print(" PPM   ");
  turnOffAllOutputs();
}

void displayCalibrateMode() {
  lcd.setCursor(0, 0); lcd.print("CALIBRATE R0?");
  lcd.setCursor(0, 1); lcd.print("Press SAVE");
  turnOffAllOutputs();
}