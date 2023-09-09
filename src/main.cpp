#include <Arduino.h>
#include <Wire.h>
#include "NtcSensor.h"



#define BATTERY_12V_ADC A0
#define BATTERY_8V_ADC A6
#define BATTERY_4V_ADC A7
#define TEMP_V_ADC A1
#define SOLAR_V_ADC A2
// Ignore the precision, its not relevant or valid, 
// Max voltage = 3.3*(top+bottom)/bottom
// mv/bit = (3.3/1024)*(top+bottom)/bottom
#define BATTERY_12V_SCALE 18.834778  // Top=100K/ Bottom=22 maxv=18.3 mv/bit= 17.87109375
// adjustment 17.87109375*12110/11491=18.833778201

#define BATTERY_8V_SCALE 10.521510       // Top=100K bottom=47 maxv = 10.3212765957, mv/bit = 10.0793716755
// adjustment 10.079371676*8070/7730.88=10.521509767
#define BATTERY_4V_SCALE 4.920638    // Top=46K Bottom=100K  direct    maxv = 6.1097826087, mv/bit = 5.9665845788
// adjustment 5.9665845788*4030/4886.63=4.92063771
#define SOLAR_SCALE 18.804355       // Top=100K/ Bottom=22 maxv=18.3 mv/bit= 17.87109375
// adjustment 17.87109375*12110/11508.98=18.804354974

#define SOLAR_EN 2
#define PUMP_EN 3

#define STANDBY 0
#define CHARGING 1
#define DISCHARGING 2

float adc4v, adc8v, packVoltage;
int16_t lightLevel = 500;


int chargeState = CHARGING;
int pumpEnable = HIGH;
int solarEnable = HIGH;

NtcSensor temperature(TEMP_V_ADC);




void showHelp() {
  Serial.println(F("Lipo voltage switch"));
  Serial.println(F("  - Send 'h' to show this message"));
  Serial.println(F("  - Send 's' to show status"));
  Serial.println(F("  - Send 'o' to toggle output"));
  Serial.println(F("  - Send 'c' to toggle charge"));
}

void heading(const __FlashStringHelper * txt) {
  Serial.print(txt);
  uint8_t l = 35 - strlen_P((const char*)txt);
  while(l> 0 ) {
    Serial.print(' ');
    l--;
  }
  Serial.print("");
}




void readSensors() {
  adc4v = BATTERY_4V_SCALE*analogRead(BATTERY_4V_ADC);
  delay(100);
  adc8v = BATTERY_8V_SCALE*analogRead(BATTERY_8V_ADC);
  delay(100);
  packVoltage = BATTERY_12V_SCALE*analogRead(BATTERY_12V_ADC);
  delay(100);
  lightLevel = analogRead(SOLAR_V_ADC);
  delay(100);
  temperature.update();
}


void showStatus() {
  readSensors();
  heading(F("Light Level"));Serial.println(lightLevel);
  heading(F("Pack Voltage (mv)"));Serial.println(packVoltage);
  heading(F("ADC 4v"));Serial.println(adc4v);
  heading(F("ADC 8v"));Serial.println(adc8v);
  heading(F("ADC 12v"));Serial.println(packVoltage);
  heading(F("Cell 1 (mv)"));Serial.println(adc4v);
  heading(F("Cell 2 (mv)"));Serial.println(adc8v-adc4v);
  heading(F("Cell 3 (mv)"));Serial.println(packVoltage-adc8v);
  heading(F("Temperature (C)"));Serial.println(temperature.getC());
  heading(F("Temperature (adc)"));Serial.println(temperature.getReading());
  heading(F("Pump"));Serial.println(pumpEnable?"Off":"On");
  heading(F("Charge"));Serial.println(solarEnable?"Off":"On");
}



void checkCommand() {
  if (Serial.available()) {
    char chr = Serial.read();
    switch ( chr ) {
      case 'h': showHelp(); break;
      case 's': showStatus(); break;
      case 'o': 
        pumpEnable = pumpEnable?LOW:HIGH;
        digitalWrite(PUMP_EN, pumpEnable);  
        break;
      case 'c': 
        solarEnable = solarEnable?LOW:HIGH;
        digitalWrite(SOLAR_EN, solarEnable); 
        break;
    }
  }
}


void setup() {
  Serial.begin(115200);
  pinMode(PUMP_EN, OUTPUT);
  digitalWrite(PUMP_EN, HIGH);
  pinMode(SOLAR_EN, OUTPUT);
  digitalWrite(SOLAR_EN, HIGH);
  showHelp();

}

bool checkOperatingConditions() {
  if ( temperature.getC() <= 4.0  ) {
    return false;
  } else if ( lightLevel > 50 ) { // to dark.
    return false;
  } else {
    return true;
  }
}



void loop() {
  static unsigned long lastUpdate = 0;
  checkCommand();
  unsigned long now = millis();
  if ( now -lastUpdate > 5000 ) {
    lastUpdate = now;
    readSensors();

      /*
      Battery state. 100% is 4.2v per cell.
      3.99 is 80%
      2.75 os 10%
      BMS will disconnect if the charge voltage goes too high, so 
      we probably dont really need to disconnect the solar.

      If the solar output is > 12.6 then the pump can be on regardless.
      Otherwise it depends on the battery state.
        If the battery is 4v per cell then pump should go on till the voltage drops to 3v/cell at which point it should 
        turn off
      */

    switch(chargeState) {
      case STANDBY:
        if ( checkOperatingConditions()  ) {
          chargeState = CHARGING;
          pumpEnable = HIGH;
          solarEnable = LOW;
          digitalWrite(PUMP_EN, pumpEnable);
          digitalWrite(SOLAR_EN, solarEnable);
          Serial.print(millis());
          Serial.println(F("Solar on "));
          showStatus();
        }
        break;
      case CHARGING:
        if ( !checkOperatingConditions() ) {
          chargeState = STANDBY;
          pumpEnable = HIGH;
          solarEnable = HIGH;
          digitalWrite(PUMP_EN, pumpEnable);
          digitalWrite(SOLAR_EN, solarEnable);
          Serial.print(millis());
          Serial.println(F("Too Cold or too dark"));
          showStatus();
        } else if (packVoltage >= 12000 ) {
          chargeState = DISCHARGING;
          pumpEnable = LOW;
          solarEnable = LOW;
          digitalWrite(PUMP_EN, pumpEnable);
          digitalWrite(SOLAR_EN, solarEnable);
          Serial.print(millis());
          Serial.println(F("Pump turned on "));
          showStatus();
        }
        break;
      case DISCHARGING:
        if ( !checkOperatingConditions() ) {
          chargeState = STANDBY;
          pumpEnable = HIGH;
          solarEnable = HIGH;
          digitalWrite(PUMP_EN, pumpEnable);
          digitalWrite(SOLAR_EN, solarEnable);
          Serial.print(millis());
          Serial.println(F("Too Cold or too dark"));
          showStatus();
        } else if (packVoltage <= 10500 ) {
          chargeState = CHARGING;
          pumpEnable = HIGH;
          solarEnable = LOW;
          digitalWrite(PUMP_EN, HIGH);
          digitalWrite(SOLAR_EN, LOW);
          Serial.print(millis());
          Serial.println(F("Pump turned off "));
          showStatus();
        }
        break;
    }      
    for(int i = 0; i < (chargeState+1); i++) {
      digitalWrite(13,HIGH);
      delay(100);
      digitalWrite(13,LOW);
      delay(100);
    }
  }
}