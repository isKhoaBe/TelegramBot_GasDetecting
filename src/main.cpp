#include <Arduino.h>
#include <Wire.h>
#include <WiFi.h>
#include "config.h"
#include "fsm_buttons.h"
#include "mq2_sensor.h"
#include "ui_display.h"
#include "telegram_task.h"

// --- Volatile parameters ---
volatile float R0 = DEFAULT_RO;
volatile int gasPPM = 0;
volatile int warnThreshold = DEFAULT_WARN_PPM;
volatile int criticalThreshold = DEFAULT_CRIT_PPM;

LiquidCrystal_I2C lcd(I2C_ADDR, 16, 2);
Preferences preferences;
WiFiClientSecure gasTelegramClient;

// ================= SETUP =================
void setup()
{
  Serial.begin(115200);

  // Pinout Config
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(BTN_MENU, INPUT_PULLUP);
  pinMode(BTN_UP, INPUT_PULLUP);
  pinMode(BTN_DOWN, INPUT_PULLUP);
  pinMode(BTN_SAVE, INPUT_PULLUP);

  // ADC Config
  analogReadResolution(12);
  analogSetAttenuation(ADC_11db);

  lcd.init();
  lcd.backlight();

  if (digitalRead(BTN_MENU) == LOW)
  {
    lcd.setCursor(0, 0);
    lcd.print("FACTORY RESET...");
    preferences.begin("gas-config", false);
    preferences.clear();
    preferences.end();

    digitalWrite(LED_GREEN, HIGH);
    digitalWrite(BUZZER, HIGH);
    delay(1000);
    digitalWrite(BUZZER, LOW);
    digitalWrite(LED_GREEN, LOW);

    lcd.setCursor(0, 1);
    lcd.print("DONE! REBOOTING");
    delay(2000);
    ESP.restart();
  }

  // Load Config
  preferences.begin("gas-config", false);
  warnThreshold = preferences.getInt("warn", DEFAULT_WARN_PPM);
  criticalThreshold = preferences.getInt("crit", DEFAULT_CRIT_PPM);
  R0 = preferences.getFloat("r0", DEFAULT_RO);

  lcd.setCursor(0, 0);
  lcd.print("WiFi Connecting");

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  int retry = 0;
  while (WiFi.status() != WL_CONNECTED && retry < 20)
  {
    Serial.print(".");
    retry++;
    delay(500);
  }

  lcd.clear();

  if (WiFi.status() == WL_CONNECTED)
  {
    lcd.print("WiFi OK!");
    gasTelegramClient.setInsecure();
  }
  else
  {
    lcd.print("WiFi Failed!");
  }
  delay(1000);
  lcd.clear();

  // ================= Telegram (CORE 0) =================
  xTaskCreatePinnedToCore(
      taskTelegram,
      "TelegramTask",
      10000,
      NULL,
      1,
      NULL,
      0);
}

// ================= LOOP (CORE 1) =================
void loop()
{
  handleButtons();
  gasPPM = readMQ2_PPM_Smoothed();

  switch (currentScreen)
  {
  case SCREEN_MONITOR:
    runMonitorMode();
    break;
  case SCREEN_SET_WARN:
    displaySettingMode("SET WARNING PPM", warnThreshold);
    break;
  case SCREEN_SET_CRITICAL:
    displaySettingMode("SET CRITICAL PPM", criticalThreshold);
    break;
  case SCREEN_CALIBRATE:
    displayCalibrateMode();
    break;
  }

  Serial.println("ADC: ");
  Serial.println(analogRead(MQ2_PIN));
  delay(50);
}