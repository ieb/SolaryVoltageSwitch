#include "BQ4050.h"
#include "Wire.h"


void BQ4050::begin() {
  Wire.begin();
}


#define swapByteOrder(v) ((v[0]) | (v[1]<<8))


void BQ4050::readBattery() {
   fullChargeCapacity =  readUint16(0x10);
   stateOfCharge = readUint8(0x0E);
   packVoltage = readUint16(0x09); // in mV
   remainingCapacity = readUint16(0x0F); // in mWh
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

void BQ4050::showBatteryStatus() {
      uint16_t batteryStatus = readWord(0x16);
    Serial.println(F("BatteryStatus"));

    heading(F("Overcharge")); Serial.println(( (batteryStatus&0x8000) == 0x8000 )?"Y":"N");
    heading(F("Terminate Charge Alarm")); Serial.println(( (batteryStatus&0x4000) == 0x4000 )?"Y":"N");
    heading(F("Over Temperature Alarm")); Serial.println(( (batteryStatus&0x1000) == 0x1000 )?"Y":"N");
    heading(F("Terminate DisCharge Alarm")); Serial.println(( (batteryStatus&0x0800) == 0x0800 )?"Y":"N");
    heading(F("Remaining Capacity Alarm")); Serial.println(( (batteryStatus&0x0200) == 0x0200 )?"Y":"N");
    heading(F("Remaining Time Alarm")); Serial.println(( (batteryStatus&0x0100) == 0x0100 )?"Y":"N");
    heading(F("Initialisation Complete")); Serial.println(( (batteryStatus&0x0080) == 0x0080 )?"Y":"N");
    heading(F("Mode")); Serial.println(( (batteryStatus&0x0040) == 0x0040 )?"Discharge":"Charge");
    heading(F("Charged")); Serial.println(( (batteryStatus&0x0020) == 0x0020 )?"Full":"not full");
    heading(F("Discharged")); Serial.println(( (batteryStatus&0x0010) == 0x0010 )?"depleated":"not depleated");
    heading(F("Error"));
    switch(batteryStatus&0x0F) {
      case 0:  Serial.println("OK"); break;
      case 1:  Serial.println("Busy"); break;
      case 2:  Serial.println("Reserved Command"); break;
      case 3:  Serial.println("Unsupported Command"); break;
      case 4:  Serial.println("AccessDenied"); break;
      case 5:  Serial.println("Overflow/Underflow"); break;
      case 6:  Serial.println("BadSize"); break;
      case 7:  Serial.println("Unknown"); break;
    } 
}

void BQ4050::showBattery() {

    uint16_t batteryMode = readWord(0x03);

    if ( (batteryMode&0x8000) == 0x8000 ) {
      units = "mW";
      unitsH = "mWh";
      scale = 10.0;
    } 
    if ( (batteryMode&0x80) == 0x80 ) {
      Serial.println(F("Charge Controller Enabled"));
    } else {
      Serial.println(F("Charge Controller Disabled"));
    }
    if ( (batteryMode&0x40) == 0x40) {
      Serial.println(F("Conditioning cycle requested"));
    } else {
      Serial.println(F("Battery OK"));
    }


    heading(F("Remaining Cacpity Alarm"));
    Serial.print((float)((scale*readUint16(0x01))));
    Serial.println(unitsH);
    heading(F("Remaining Time Alarm"));
    Serial.print((float)((readUint16(0x02))));
    Serial.println("min");
    heading(F("At Rate"));
    Serial.print((float)((scale*readInt16(0x04))));
    Serial.println(units);
    heading(F("At Rate time to full"));
    Serial.print((float)((scale*readUint16(0x05))));
    Serial.println(units);
    heading(F("At Rate time to empty"));
    Serial.print((float)((scale*readUint16(0x06))));
    Serial.println(units);
    heading(F("At Rate Ok"));
    Serial.println(readUint16(0x07)?"No":"Yes");
    
    heading(F("Pack Temperature"));
    Serial.print((float)((0.1*readUint16(0x08))-273.15));
    Serial.println("C");
    heading(F("Pack Voltage"));
    Serial.print((float)(0.001*readUint16(0x09)));
    Serial.println("V");
    heading(F("Current"));
    Serial.print((float)((0.001*readInt16(0x0A))));
    Serial.println("A");
    heading(F("Average Current"));
    Serial.print((float)(0.001*readInt16(0x0B)));
    Serial.println("A");
    heading(F("Max State of Charge Error"));
    Serial.print(readUint8(0x0C));
    Serial.println("%");
    heading(F("Absolute State of Charge"));
    Serial.print(readUint8(0x0E));
    Serial.println("%");
    heading(F("Remaining Capacity"));
    Serial.print((float)(scale*readUint16(0x0F)));
    Serial.println(unitsH);
    heading(F("Full Charge Capacity"));
    Serial.print((float)(scale*readUint16(0x10)));

    Serial.println(unitsH);
    heading(F("Runtime to Empty"));
    Serial.print(readUint16(0x10));
    Serial.println("min");
    heading(F("Desired Charging Current"));
    Serial.print((float)(1.0*readUint16(0x14)));
    Serial.println("mA");
    heading(F("Desired Charging Voltage"));
    Serial.print((float)(1.0*readUint16(0x15)));
    Serial.println("mV");
    
    showBatteryStatus();
    heading(F("Cycle Count"));
    Serial.println(readUint16(0x17));
    heading(F("Design Capacity"));
    Serial.print((float)(scale*readUint16(0x18)));
    Serial.println(unitsH);
    heading(F("Design Voltage"));
    Serial.print((float)(1.0*readUint16(0x19)));
    Serial.println("mV");

    heading(F("Cell 4"));
    Serial.print((float)(1.0*readUint16(0x3C)));
    Serial.println("mV");
    heading(F("Cell 3"));
    Serial.print((float)(1.0*readUint16(0x3D)));
    Serial.println("mV");
    heading(F("Cell 2"));
    Serial.print((float)(1.0*readUint16(0x3E)));
    Serial.println("mV");
    heading(F("Cell 1"));
    Serial.print((float)(1.0*readUint16(0x3F)));
    Serial.println("mV");
}

uint16_t BQ4050::readUint16(uint8_t addr) {
  uint8_t value[2];
  readDataReg(i2cAddress, addr, value, 2 );
  return swapByteOrder(value);
}
int16_t BQ4050::readInt16(uint8_t addr) {
  uint8_t value[2];
  readDataReg(i2cAddress, addr, value, 2 );
  return swapByteOrder(value);
}

uint8_t BQ4050::readUint8(uint8_t addr) {
  uint8_t value;
  readDataReg(i2cAddress, addr, &value, 1 );
  return value;
}

uint16_t BQ4050::readWord(uint8_t addr) {
  uint16_t value;
  readDataReg(i2cAddress, addr, (uint8_t *)&value, 2 );
  return value;
}




boolean BQ4050::readDataReg( uint8_t devAddress, uint8_t regAddress, uint8_t *dataVal, uint8_t arrLen ){

  //Function to read data from the device registers

  //Add in the device address to the buffer
  Wire.beginTransmission( devAddress );
  //Add the one byte register address
  Wire.write( regAddress );
  //Send out buffer and log response
  byte ack = Wire.endTransmission();
  //If data is ackowledged, proceed
  if( ack == 0 ){
    //Request a number of bytes from the device address
    Wire.requestFrom( devAddress , arrLen );
    //If there is data in the in buffer
    if( Wire.available() > 0 ){
      //Cycle through, loading data into array
      for( uint8_t i = 0; i < arrLen; i++ ){
        dataVal[i] = Wire.read();
      }
    }
    return true;
  }else{
    return false; //if I2C comm fails
  }
}