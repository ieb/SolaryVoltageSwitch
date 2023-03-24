#include <Arduino.h>
#include <Wire.h>

#include "BQ4050.h"
#define BQ4050addr     0x0B

#define BATTERY_ADC A0
#define PUMP_RELAY_PIN 3
#define CHARGING 0
#define DISCHARGING 1


BQ4050 batteryController(BQ4050addr);
int chargeState = CHARGING;
int flashes = 1;







void showHelp() {
  Serial.println(F("Lipo voltage switch"));
  Serial.println(F("  - Send 'h' to show this message"));
  Serial.println(F("  - Send 's' to show status"));
  Serial.println(F("  - Send 'i' to interogate the i2c bus"));
}

void showStatus() {
  int16_t adc0 = analogRead(BATTERY_ADC);
  float solarVoltage = (12.20*adc0/652.0);
  batteryController.showBattery();
  Serial.print("Solar Voltage :");
  Serial.println(solarVoltage);
  Serial.print("Switch State :");
  switch(chargeState) {
    case CHARGING: Serial.println("Chaging, relay off"); break;
    case DISCHARGING: Serial.println("Dischaging, relay on"); break;
  }  
}

void scanI2C() {
  Serial.println("I2C scanning ");
  for (uint8_t addr = 0; addr < 255; addr++) {
      Wire.beginTransmission(addr);
      if (Wire.endTransmission() == 0) {
          Serial.print(" ");
          Serial.print(addr,HEX);
      } else {
          Serial.print(" --");
      }
  }
  Serial.println("Done ");

}
void checkCommand() {
  if (Serial.available()) {
    char chr = Serial.read();
    switch ( chr ) {
      case 'h': showHelp(); break;
      case 's': showStatus(); break;
      case 'i': scanI2C(); break;
    }
  }
}


void setup() {
  Serial.begin(115200);
  Serial.print(" Doing startup ");
  batteryController.begin();
  pinMode(PUMP_RELAY_PIN, OUTPUT);
  digitalWrite(PUMP_RELAY_PIN, HIGH);
  showHelp();
}


void loop() {
  static unsigned long lastUpdate = 0;
  checkCommand();
  unsigned long now = millis();
  if ( now > lastUpdate+5000 ) {
    lastUpdate = now;
    batteryController.readBattery();
    int16_t adc0 = analogRead(BATTERY_ADC);
    switch (chargeState) {
      case CHARGING:
        if ( batteryController.getPercentCharge() > 80 && adc0 >  641 /*12v*/) {
          flashes = 3;
          chargeState = DISCHARGING;
          digitalWrite(PUMP_RELAY_PIN, LOW);
          Serial.print(millis());
          Serial.print(F(" Charge:"));
          Serial.print(batteryController.getPercentCharge());
          Serial.print(F("% Solar:"));
          Serial.print((12.20*adc0/652.0));
          Serial.println(F("V Pump on"));
        }
        break;
      case DISCHARGING:
        if ( batteryController.getPercentCharge() < 30 || adc0 < 641 /*12v*/ ) {
          flashes = 1;
          chargeState = CHARGING;
          digitalWrite(PUMP_RELAY_PIN, HIGH);
          Serial.print(millis());
          Serial.print(F(" Charge:"));
          Serial.print(batteryController.getPercentCharge());
          Serial.print(F("% Solar:"));
          Serial.print((12.20*adc0/652.0));
          Serial.println(F("V Pump off"));
        }
        break;
    }
    for(int i = 0; i < flashes; i++) {
      digitalWrite(13,HIGH);
      delay(100);
      digitalWrite(13,LOW);
      delay(100);
    }
  }
}