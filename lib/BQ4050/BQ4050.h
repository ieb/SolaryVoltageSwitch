#pragma once

#include <Arduino.h>

/**
 * Readonly interface to a TI BR4050 Lipo battery guage, found in many laptop batteries.
 * May also work woth other chip that have the same register set 
 */

class BQ4050 {
public:
	BQ4050(uint8_t address) {
		this->i2cAddress = address;
	};
	void begin();
	void showBattery();
	void readBattery();
	uint8_t getPercentCharge() {
		return stateOfCharge;
	}
private:
	uint8_t i2cAddress;
	const char * units = "mA";
	const char * unitsH = "mAh";
	float scale = 1.0;
	uint16_t fullChargeCapacity;
   	uint16_t packVoltage; // in mV
   	uint16_t remainingCapacity; // in mWh
	uint8_t stateOfCharge;
	void showBatteryStatus();
	uint16_t readUint16(uint8_t addr);
	int16_t readInt16(uint8_t addr);
	uint8_t readUint8(uint8_t addr);
	uint16_t readWord(uint8_t addr);
	boolean readDataReg( uint8_t devAddress, uint8_t regAddress, uint8_t *dataVal, uint8_t arrLen );
};