#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <Preferences.h>
#include <WiFiClientSecure.h>

// --- HARDWARE CONFIG ---
#define I2C_ADDR 0x27
#define MQ2_PIN 34
#define RL_VALUE 1.0

// Output Pins
#define LED_GREEN 18
#define LED_YELLOW 19
#define LED_RED 23
#define BUZZER 5

// Input Pins
#define BTN_MENU 13
#define BTN_UP 12
#define BTN_DOWN 14
#define BTN_SAVE 27

// --- MQ2 COEFFICIENTS ---
const float RO_CLEAN_AIR_FACTOR = 9.83;
const float CURVE_A = 565.46;
const float CURVE_B = -2.203;

// --- DEFAULT SETTING ---
const int DEFAULT_WARN_PPM = 500;
const int DEFAULT_CRIT_PPM = 1500;
const float DEFAULT_RO = 10.0;

// --- WIFI & TELEGRAM CONFIG ---
extern const char *WIFI_SSID;
extern const char *WIFI_PASSWORD;
extern const char *GAS_BOT_TOKEN;
extern const char *GAS_CHAT_ID;

// --- VOLATILE ---
extern volatile float R0;
extern volatile int gasPPM;
extern volatile int warnThreshold;
extern volatile int criticalThreshold;

extern LiquidCrystal_I2C lcd;
extern Preferences preferences;
extern WiFiClientSecure gasTelegramClient;

#endif // CONFIG_H