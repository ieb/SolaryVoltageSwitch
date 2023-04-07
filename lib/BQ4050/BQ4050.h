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
	void sendCommand(uint16_t command);
	void readBlock(uint16_t address);
	uint8_t getPercentCharge() {
		return stateOfCharge;
	};
	uint16_t getVoltage() {
		return packVoltage;
	};
	int16_t getCurrent() {
		return packCurrent;
	};

	void dumpROM();
private:
	uint8_t i2cAddress;
	const char * units = "mA";
	const char * unitsH = "mAh";
	float scale = 1.0;
	uint16_t fullChargeCapacity;
   	uint16_t packVoltage; // in mV
   	uint16_t packCurrent; // in mV
   	uint16_t remainingCapacity; // in mWh
	uint8_t stateOfCharge;
	void showBatteryStatus();
	uint16_t readUint16(uint8_t addr);
	int16_t readInt16(uint8_t addr);
	uint8_t readUint8(uint8_t addr);
	uint16_t readWord(uint8_t addr);
	boolean readDataReg( uint8_t devAddress, uint8_t regAddress, uint8_t *dataVal, uint8_t arrLen );
	int readDataBlock( uint8_t devAddress, uint8_t regAddress, uint8_t *dataVal, uint8_t arrLen );
	boolean readManufacturerAccess(uint8_t devAddress, uint16_t regAddress, uint8_t *value, uint8_t len);
	boolean writeCommand( uint8_t devAddress, uint8_t command, uint16_t regAddress );
	boolean readManufacturerBlockAccess(uint8_t devAddress, uint16_t regAddress, uint8_t *value, uint8_t len);
	void printHex(uint8_t *b, uint8_t len);
	void printHex(uint8_t v);

	void printStringBlock(uint8_t *b, uint8_t len);
	boolean dumpBlock( uint8_t regAddress );
	void showError();






};