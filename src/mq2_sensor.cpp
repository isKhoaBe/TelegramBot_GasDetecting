#include "mq2_sensor.h"
#include "config.h"
#include <math.h>

int readMQ2_PPM_Smoothed() {
  long adcSum = 0;
  int sampleCount = 30; 
  
  for(int i=0; i<sampleCount; i++) {
    adcSum += analogRead(MQ2_PIN);
    delayMicroseconds(500); 
  }
  
  int adcRaw = adcSum / sampleCount;

  if (adcRaw < 50) return 0; 

  float voltage = adcRaw * (3.3 / 4095.0);
  float RS_gas = (3.3 * RL_VALUE / voltage) - RL_VALUE;
  
  if (R0 <= 0) R0 = DEFAULT_RO; 
  float ratio = RS_gas / R0;
  if (ratio <= 0) ratio = 0.01;

  float ppmLog = CURVE_A * pow(ratio, CURVE_B);
  
  if (ppmLog < 0) ppmLog = 0;

  if (ppmLog > 100000) ppmLog = 100000;
  return (int)ppmLog;
}

void calibrateSensor() {
  lcd.clear();
  lcd.setCursor(0,0); lcd.print("Calibrating...");
  
  float valTotal = 0;
  for(int i=0; i<50; i++) {
    int adc = analogRead(MQ2_PIN);
    if(adc < 10) { i--; continue; } 
    float volts = adc * (3.3 / 4095.0);
    float RS_air = (3.3 * RL_VALUE / volts) - RL_VALUE;
    valTotal += RS_air;
    delay(50);
  }
  
  float RS_air_avg = valTotal / 50.0;
  float R0_new = RS_air_avg / RO_CLEAN_AIR_FACTOR;
  
  if (R0_new > 0 && R0_new < 100) { 
      R0 = R0_new;
      preferences.putFloat("r0", R0);
      lcd.setCursor(0,1); lcd.print("Done! R0="); lcd.print(R0);
  } else {
      lcd.setCursor(0,1); lcd.print("Failed! R0 invalid");
  }
  delay(2000);
}