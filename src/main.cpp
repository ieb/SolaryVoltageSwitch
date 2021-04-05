#include <Arduino.h>

#define BATTERY_ADC A0
#define ADC_SCALE (12200.0/(2200.0*1024))
#define PUMP_RELAY_PIN 3
void setup() {
  Serial.begin(9600);
  pinMode(PUMP_RELAY_PIN, OUTPUT);
  digitalWrite(PUMP_RELAY_PIN, HIGH);
}

bool pumpOn = false;

void loop() {
  int16_t adc0 = analogRead(BATTERY_ADC);
  float batteryVoltage = (12.20*adc0/652.0);
  Serial.println(adc0);
  Serial.println(batteryVoltage);


  // if voltage is > 13.6 turn on < 12.5 turn off.
  if ( !pumpOn && batteryVoltage > 14 ) {
    digitalWrite(PUMP_RELAY_PIN, LOW);
    pumpOn = true;
  } else if ( pumpOn && batteryVoltage < 12.4 ) {
    digitalWrite(PUMP_RELAY_PIN, HIGH);
    pumpOn = false;
  }
  if ( pumpOn ) {
    digitalWrite(13,HIGH);
    delay(100);
    digitalWrite(13,LOW);
    delay(100);
    digitalWrite(13,HIGH);
    delay(100);
    digitalWrite(13,LOW);
  } else {
    digitalWrite(13,HIGH);
    delay(100);
    digitalWrite(13,LOW);
  }
  delay(1000);
}