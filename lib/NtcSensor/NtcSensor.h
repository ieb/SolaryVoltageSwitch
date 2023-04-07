
#pragma once

#include <Arduino.h>

/**
 * @brief ADC readings at temperatures from MIN_NTC_TEMPERATURE to MAX_NTC_TEMPERATURE in steps of NTC_TABLE_STEP
 * calculated using https://www.skyeinstruments.com/wp-content/uploads/Steinhart-Hart-Eqn-for-10k-Thermistors.pdf
 * https://www.gotronic.fr/pj2-mf52type-1554.pdf MC53-10K B 3950K
 * Vtop doesnt matter if the ADC reference is VTop. To check this try changing Vtop in a spreadsheet.
ADC 1024        
Vtop    3.3     
Rtop    20.1        
C   R   V   ADC
-10 56.06   2.4290703782    753.7478991597
0   32.96   2.0499057671    636.0919713532
10  20  1.6458852868    510.72319202
20  12.51   1.2659613615    392.8316467341
30  8.048   0.9435270712    292.7793093648
40  5.312   0.689815835 214.0519439635
50  3.588   0.4998480243    155.1043566363
60  2.476   0.3619241673    112.3061658398
70  1.743   0.263329213 81.7118527675
80  1.25    0.1932084309    59.9531615925
90  0.911   0.143082195 44.3988387035
100 0.6744  0.1071280037    33.2421441774
110 0.5066  0.0811283763    25.1743810236


ADC reading needs to be referenced to the supply voltage, to account for 
supply voltage changes.

 */
const int16_t ntcTable[] PROGMEM= {
    754,
    636,
    511,
    393,
    293,
    214,
    155,
    112,
    82,
    60,
    44,
    33,
    25
};
#define NTC_TABLE_LENGTH 13
#define MIN_NTC_TEMPERATURE -10.0
#define MAX_NTC_TEMPERATURE 110.0
#define NTC_TABLE_STEP 10.0

//#define DEBUG 1

class NtcSensor {
public:
    NtcSensor(uint8_t pin){  this->pin = pin;};
    void update() {
        reading = analogRead(pin);

        temperature = MAX_NTC_TEMPERATURE;
        int16_t cvp = ((int16_t)pgm_read_dword(&ntcTable[0]));
        if ( reading > cvp ) {
            temperature = MIN_NTC_TEMPERATURE;
        } else {
            for (int i = 1; i < NTC_TABLE_LENGTH; i++) {
                int16_t cv = ((int16_t)pgm_read_dword(&ntcTable[i]));
                if ( reading > cv ) {
                    temperature = ((cvp-reading)*NTC_TABLE_STEP);
                    temperature /= (cvp-cv);
                    temperature += ((i-1)*NTC_TABLE_STEP)+MIN_NTC_TEMPERATURE;
                    break;
                }
                cvp = cv;
            }
        }
#ifdef DEBUG
        Serial.print(F("Sensor "));
        Serial.print(n);
        Serial.print(F(" pin="));
        Serial.print(pin);
        Serial.print(F(" adc="));
        Serial.print(reading);
        Serial.print(F(" temp="));
        Serial.println(temperature);
#endif
    };
    float getC() {
        return temperature;
    };
    int16_t getReading() {
        return reading;
    };

private:
    uint8_t pin;
    int16_t reading;
    float temperature;
};
