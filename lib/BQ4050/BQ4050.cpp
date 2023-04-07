#include "BQ4050.h"
#include "Wire.h"

/*
 * Although this code works for interacting with a BMS based on a BQ4050 I am not using it because
 * the BQ4050 is hard to program and I dont have confidence it wont lock me out from the battery.
 * 
 */


void BQ4050::begin() {
  Wire.begin();
}


#define swapByteOrder(v) ((v[0]) | (v[1]<<8))


void BQ4050::readBattery() {
   fullChargeCapacity =  readUint16(0x10);
   stateOfCharge = readUint8(0x0E);
   packVoltage = readUint16(0x09); // in mV
   packCurrent = readInt16(0x0A); // in mA
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

void testDevice(uint8_t addr, int len) {
	Serial.println("Test");
	Serial.print(addr,HEX);
	Serial.println(" <send");
	Wire.beginTransmission(0x0B);
	Wire.write(0x44);
	Wire.write(addr);
	Wire.write(0x00);
	Wire.endTransmission();
	delay(5);
	Wire.requestFrom( 0x0B , len );
	 if( Wire.available() > 0 ){
      //Cycle through, loading data into array
      for( uint8_t i = 0; i < len; i++ ){
        Serial.println(Wire.read(),HEX);
      }
    }

	Serial.println("Test Done");

}

void BQ4050::showError() {
	uint16_t batteryStatus = readWord(0x16);
    heading(F("Error"));
    switch((batteryStatus>>8)&0x0F) {
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

void BQ4050::showBatteryStatus() {
      uint16_t batteryStatus = readWord(0x16);
    heading(F("BatteryStatus 0x16")); printHex((uint8_t *)&batteryStatus, 2); Serial.println("");

    heading(F("Overcharge")); Serial.println(( (batteryStatus&0x80) == 0x80 )?"Y":"N");
    heading(F("Terminate Charge Alarm")); Serial.println(( (batteryStatus&0x40) == 0x40 )?"Y":"N");
    heading(F("Over Temperature Alarm")); Serial.println(( (batteryStatus&0x10) == 0x10 )?"Y":"N");
    heading(F("Terminate DisCharge Alarm")); Serial.println(( (batteryStatus&0x08) == 0x08 )?"Y":"N");
    heading(F("Remaining Capacity Alarm")); Serial.println(( (batteryStatus&0x02) == 0x02 )?"Y":"N");
    heading(F("Remaining Time Alarm")); Serial.println(( (batteryStatus&0x01) == 0x01 )?"Y":"N");
    heading(F("Initialisation Complete")); Serial.println(( (batteryStatus&0x8000) == 0x8000 )?"Y":"N");
    heading(F("Mode")); Serial.println(( (batteryStatus&0x4000) == 0x4000 )?"Discharge":"Charge");
    heading(F("Charged")); Serial.println(( (batteryStatus&0x2000) == 0x2000 )?"Full":"not full");
    heading(F("Discharged")); Serial.println(( (batteryStatus&0x1000) == 0x1000 )?"depleated":"not depleated");
    heading(F("Error"));
    switch((batteryStatus>>8)&0x0F) {
      case 0:  Serial.println("OK"); break;
      case 1:  Serial.println("Busy"); break;
      case 2:  Serial.println("Reserved Command"); break;
      case 3:  Serial.println("Unsupported Command"); break;
      case 4:  Serial.println("AccessDenied"); break;
      case 5:  Serial.println("Overflow/Underflow"); break;
      case 6:  Serial.println("BadSize"); break;
      case 7:  Serial.println("Unknown"); break;
    } 



 	uint8_t value[40];

 	readDataReg(i2cAddress, 0x1C, value, 2 );
  	heading(F("SerialNumber")); printHex(value,2); Serial.println("");

  	value[20] = '\0';
  	readDataBlock(i2cAddress, 0x20, value, 20 );
  	heading(F("ManufacturerName")); Serial.print((const char *)&value[0]); Serial.println("");
  	value[20] = '\0';
  	readDataBlock(i2cAddress, 0x21, value, 20 );
  	heading(F("DeviceName")); Serial.print((const char *)&value[0]); Serial.println("");
  	readDataBlock(i2cAddress, 0x22, value, 5 );
  	heading(F("DeviceChemistry")); Serial.print((const char *)&value[0]); Serial.println("");
  	heading(F("ManufacturerData")); dumpBlock(0x23);Serial.println("");
  	heading(F("Authenticate")); dumpBlock(0x2F); Serial.println("");


  	readDataReg(i2cAddress, 0x4A, value, 2 );
  	heading(F("BTPDischargeSet 0x4A")); printHex(value,2); Serial.println("");
  	// 0x96 0x00 = 150 int, 
  	readDataReg(i2cAddress, 0x4F, value, 2 );
  	heading(F("StateOfHealth 0x4F")); printHex(value,2); Serial.println("");
  	// 6400 ==  100 int
  	int n = readDataBlock(i2cAddress, 0x50, value, 4 );
  	heading(F("Safety Alert 0x50")); printHex(value,4); Serial.println("");
  	// 00000100 00000000 00000000 00000000
  	//     26 Under temp during charge
  	n = readDataBlock(i2cAddress, 0x51, value, 4 );
  	heading(F("SafetyStatus  0x51")); printHex(value,n); Serial.println("");
  	// 00000100 00000000 00000000 00000000
  	//       0x00     0x04     0x00     0x00
  	n = readDataBlock(i2cAddress, 0x52, value, 4 );
  	heading(F("PFAlert 0x52 ")); printHex(value,n); Serial.println("");
  	// 00000100 00000000 00000000 00000000
  	n = readDataBlock(i2cAddress, 0x53, value, 4 );
  	heading(F("PFStatus  0x53")); printHex(value,n); Serial.println("");
  	// 00000100 00000000 00000000 00000000
    // operation status
  	n = readDataBlock(i2cAddress, 0x54, value, 4 );
  	heading(F("Operatonal Status 0x54")); printHex(value,n); Serial.println("");
  	heading(F("   Presence")); Serial.println(( (value[0] & 0x01)  == 0x01)?F("active"):F("inactive")); 
  	heading(F("   DSC Fet")); Serial.println(( (value[0] & 0x02) == 0x02 )?F("active"):F("inactive")); 
  	heading(F("   CHG Fet")); Serial.println(( (value[0] & 0x04) == 0x04 )?F("active"):F("inactive")); 
  	heading(F("   PCH Fet")); Serial.println(( (value[0] & 0x08) == 0x08 )?F("active"):F("inactive")); 
  	heading(F("   Reserved access")); Serial.println(( (value[1] & 0x03) == 0x00 )?F("Y"):F("N")); 
  	heading(F("   Full access")); Serial.println(( (value[1] & 0x03) == 0x01 )?F("Y"):F("N")); 
  	heading(F("   Unsealed")); Serial.println(( (value[1] & 0x03) == 0x02 )?F("Y"):F("N")); 
  	heading(F("   Sealed")); Serial.println(( (value[1] & 0x03) == 0x03 )?F("Y"):F("N")); 
  	heading(F("   Discharing")); Serial.println(( (value[1] & 0x20) == 0x20 )?F("disabled"):F("enabled")); 
  	heading(F("   Charging")); Serial.println(( (value[1] & 0x40) == 0x40 )?F("disabled"):F("enabled")); 
  	// 00000100 10000111 00000001 00000000
  	// 10000101 00101001 00000000 00000000
  	// 10000101 00101001 00000000 00000000
  	//        0 presence actove    <<<
  	//       1  DSC Fet inactive     <<<
  	//      2   CHG Fet active     <<<
  	//     3    Precharge Fet inactive
  	//    4 reserved                
  	//   5  Fuse inactive        
  	//  6 smoothng inactive            
  	// 7 battery trip active              
  	//                98  Unsealed
  	//              10 no low voltage shutdown
  	//             11 safety mode, active.
  	//            12 permanent failure inactive PF
  	//           13 Discharge disabled active. XDSG
  	//          14 charging disabled inactive.  XCHG
  	//         15 sleep conditions met  inactive
  	//                         16 Shutdown commant active.  <<<<<
  	//                        17 Led display on ??? odd
  	//                       18 Authentication in progress active odd, perhaps 9 and 8
  	//                      19 Calibration complete
  	//                     20 Calibration data not available.
  	//                    21 Raw Calibration data available.
  	//                   22 100khz sm bus
  	//                  23 Sleep by command, active
  	//                                24 init after reset inactive
  	//                               25 Auto cal not starting
  	//                              26 ADC measured in sleep mode, active.
  	//                             27 CC measured in sleep mode, inactive.
  	//                            28 Cell balancing, inactive
  	//                           29 Emergency shutdown inactive
  	//                         unused.


  	n = readDataBlock(i2cAddress, 0x55, value, 4 );
  	heading(F("Charging Status 0x55")); printHex(value,n); Serial.println("");
  	// 00000011 00001000 00010000 00000000
    //        0 Precharge Voltage Region inactive
    //       1 Low Voltage Region inactive
    //      2 Mid Voltage Region inactive
    //     3 High Voltage Region inactive
    //    4 Charge Inhibit inactive
    //   5 harge Suspend inactive
    //  6 Maintenance Charge inactive
    // 7 Charge Termination inactive
    //8 Charging Current Rate of Change inactive
    //                         9Charging Voltage Rate of Change inactive
    //                       10  Charging Loss Compensation active
    //                14-11 reserved
    //                  15 Taper voltage not satisfied
    //              31-16 do no use



  	n = readDataBlock(i2cAddress, 0x56, value, 4 );
  	heading(F("Gauging Status 0x56")); printHex(value,n); Serial.println("");
  	// 00000010 01000000 00000000 10001000
    //                                   0 Fully Discharged no
    //                                  1 Fully Charged no
	// 	                               2 Terminate Discharge no
    //                                3 Terminate Charge detected
    //                               4 Cell Balancing not allowed
    //                              5 EDB0 not reached or not in discharge mode
    //                             6 charging detected
    //                            7 Condition cyce needed
    //                         9-8 Reserved
    //                       10 FCC value not updated.
    //                   12-11 Reserved.  
    //                    13 EDV1 not reached
    //                   14 EDV2 not reached.
    //                  15 Discharge qualified for learning not detected.                       


  	n = readDataBlock(i2cAddress, 0x57, value, 4 );
  	heading(F("Manufacturing Status")); printHex(value,n); Serial.println("");
  	heading(F("   PCH Fet Test")); Serial.println(( (value[0] & 0x01)  == 0x01)?F("enabled"):F("disabled")); 
  	heading(F("   CHG Fet Test")); Serial.println(( (value[0] & 0x02) == 0x02 )?F("enabled"):F("disabled")); 
  	heading(F("   DSG Fet Test")); Serial.println(( (value[0] & 0x04) == 0x04 )?F("enabled"):F("disabled")); 
  	heading(F("   FET control")); Serial.println(( (value[0] & 0x08) == 0x08 )?F("enabled"):F("disabled")); 

    // 00010000 00000000
    //                 0 Precharge FET teste disabled.
    //                1 Charge FET test disable
    //               2 FET Discharge test disable
    //              3 reserved
    //             4 All Fect action disabled
    //            5 Lifetime data collection off
    //           6 Permanent failure no
    //          7 Black box recorder off
    //        8 Fuse action off
    //       9 LED Display is off
    //     10 reserved
    //    11 reserved
    //   12 reserved
    //  13 reserved
    // 14 LIFETIME SPEED UP mode disabled
    //15 CALIBRATION mode disabled


  	heading(F("AFERegister")); dumpBlock(0x58); Serial.println("");
  	heading(F("LifeTimeDataBlock1")); dumpBlock(0x60); Serial.println("");
  	heading(F("LifeTimeDataBlock2")); dumpBlock(0x61); Serial.println("");
  	heading(F("LifeTimeDataBlock3")); dumpBlock(0x62); Serial.println("");
  	heading(F("LifeTimeDataBlock4")); dumpBlock(0x63); Serial.println("");
  	heading(F("LifeTimeDataBlock5")); dumpBlock(0x64); Serial.println("");
  	heading(F("ManufacturerInfo")); dumpBlock(0x70); Serial.println("");
  	heading(F("DAStatus1")); dumpBlock(0x71); Serial.println("");
  	heading(F("DAStatus2")); dumpBlock(0x72); Serial.println("");


}


void BQ4050::printHex(uint8_t *b, uint8_t len) {
	for ( int i = 0; i < len; i++ ) {
		if ( b[i] < 16 ) {
			Serial.print(" 0x0");
		} else {
			Serial.print(" 0x");
		}
		Serial.print(b[i],HEX);
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

void BQ4050::printHex(uint8_t v) {
	if ( v < 0x10 ) {
		Serial.print(0);
	}
	Serial.print(v,HEX);
}

void BQ4050::dumpROM() {
	for (uint16_t addr = 0x4000; addr < 0x5FFF;) {

	  	Wire.beginTransmission( i2cAddress );
  		Wire.write( 0x044 );
  		Wire.write( 0x02 ); // 2 bytes in the block message following here.
  		Wire.write( addr&0xff );
  		Wire.write( (addr>>8)&0xff );
  		byte ack = Wire.endTransmission();
		if ( ack == 0 ) {
			Serial.print(addr,HEX);
			Serial.print(" > ");
			//Add in the device address to the buffer
			Wire.beginTransmission( i2cAddress );
			//Add the one byte register address
			Wire.write( 0x44 );
			//Send out buffer and log response
			byte ack = Wire.endTransmission();
			if ( ack == 0) {
				Wire.requestFrom( i2cAddress , (uint8_t)1 );
				if ( Wire.available() > 0 ) {
				   	uint8_t n = Wire.read();
				   	printHex(n);
				   	Wire.requestFrom(i2cAddress, (uint8_t) (n+1));
				   	if (Wire.available() > 0) {
				   		// ignore the length we just read.
					   	Wire.read();
					   	// read the data.
					   	uint8_t addrLSB;
				   		for ( int i = 0; i < n; i++) {
				   			uint8_t v = Wire.read();
				   			if ( i == 0 ) {
				   				addrLSB = v;
				   			} else if ( i == 1 ) {
								Serial.print(":");				   				
				   				printHex(v);
				   				printHex(addrLSB);
								Serial.print(":");				   				
				   			} else {
				   				printHex(v);
					   			addr++;
					   			if ( addr%4 == 0) {
									Serial.print(" ");
					   			}				   				
				   			}
				   		}

				   } else {
					   Serial.print("No len");
				   }
			  	} else {
			  		Serial.print("No Ack");
			  	}
			}
			Serial.println("");
		} else {
			showError();
		}
	}
}

void BQ4050::readBlock(uint16_t address) {
	writeCommand(i2cAddress, 0x44, address);
	dumpBlock(0x44);
}


void BQ4050::sendCommand(uint16_t cmd){
	writeCommand(i2cAddress, 0x44, cmd);
	delay(250);
	showBatteryStatus();
	return;
/*
	// how do I know if the fet is already on ?
	// check fet control is disabled.
	uint8_t value[4];
   	readDataReg(i2cAddress, 0x57, value, 4 );
   	if ((value[3] & 0x08) == 0x08 ) {
   		Serial.println("Firmware has control of the FETs, disabling");
		writeCommand(i2cAddress, 0x44, 0x0022 );
		delay(250);
	} else {
		Serial.println("FET control is manual");
	}
   	readDataReg(i2cAddress, 0x57, value, 4 );
   	if ((value[3] & 0x08) == 0x08 ) {
   		Serial.println("Firmware still has has control of the FETs, trying turn on anyway");
   	}
	writeCommand(i2cAddress, 0x44, 0x0020);
	delay(250);
	showBatteryStatus();
	*/

}

/*
void pack(uint8_t *key, uint8_t *message, unit8_t *packed){
	// 128 bits of the auth key
	for (int i = 0; i < 16; i++) {
		packed[i] = key[i];
	}
	// 20 bytes of the message
	for (int i = 0; i < 20; i++) {
		packed[i+16] = message[i];
	}
	packed[20+16] = 0x80;  // 1 + 7 zeros, 159-7 = 152 more zero bitsrequired 152/8=19 bytes
	for (int i = 0; i < 19; i++) {
		packed[20+16+1+i] = 0;
	}
		packed[20+16+1+19] = 10010000;
	for (int i = 20+16+1+19+1; i < 64; i++) {
		packed[i] = 0;
	}	
}

void doAuthentcation() {
	char packed[64];
	char message[20]; // SHA-1 HMAC containing LSB in data[0]
	char key[16];
	pack(key,message,packed);
	sha1(packed,message);
	pack(key,message,packed);
	sha1(packed,message);
	writeCommand(i2cAddress, 0x2F, 0x0037, message, 20);
	delay(250);
	// should be the same as the message
	dumpBlock(0x2F);
}
*/



boolean BQ4050::readManufacturerAccess(uint8_t devAddress, uint16_t regAddress, uint8_t *value, uint8_t len) {
	writeCommand(devAddress, 0x00, regAddress);
	uint8_t buffer[len];
  	readDataReg(devAddress, 0x06, buffer, len);
  	uint16_t reg = swapByteOrder(buffer);
  	if ( reg != regAddress ) {
  		Serial.print("Managemet Block read wrong address");
  		Serial.println(reg);
  	}
	for ( int i = 0; i < len; i++) {
		value[i] = buffer[i];
	}
	printHex(value, len);
  	return true;
}

boolean BQ4050::writeCommand( uint8_t devAddress, uint8_t command, uint16_t regAddress ) {
  printHex((uint8_t *)&command, 1);
  printHex((uint8_t *)&regAddress, 2);
  Serial.println( " < command");
  Wire.beginTransmission( devAddress );
  Wire.write( command );
  Wire.write( 0x02 ); // 2 bytes in the block message following here.
  Wire.write( regAddress&0xff );
  Wire.write( (regAddress>>8)&0xff );
  byte ack = Wire.endTransmission();
  if( ack == 0 ){
  	Serial.println("Ack recieved");
    return true;
  }else{
  	Serial.println("NoAck recieved");
  	showError();
    return false; //if I2C comm fails
  }
}

boolean BQ4050::readManufacturerBlockAccess(uint8_t devAddress, uint16_t regAddress, uint8_t *value, uint8_t len) {
	writeCommand(devAddress, 0x44, regAddress);
	uint8_t buffer[len+2];
  	readDataReg(devAddress, 0x44, buffer, len+2);
  	uint16_t reg = swapByteOrder(buffer);
  	if ( reg != regAddress ) {
  		Serial.print("Managemet Block read wrong address");
  		Serial.println(reg);
  	}
	for ( int i = 0; i < len; i++) {
		value[i] = buffer[i+2];
	}
	printHex(value, len);
  	return true;
}

/*
0x457b bit 2 0 == removable, 1 == fixed.

1E = 11110, so its already fixed and not removable.


4560 > 22:4560:C409540B 100EA00F 00324B01 C8004000 C8002801 01C00D68 1002001E 03FFFFFF 
                0 1 2 3  4 5 6 7  8 9 A B  C D E F  0 1 2 3  4 5 6 7  8 9 A B  C D E F

*/

boolean BQ4050::dumpBlock( uint8_t regAddress ){
 //Add in the device address to the buffer
  Wire.beginTransmission( i2cAddress );
  //Add the one byte register address
  Wire.write( regAddress );
  //Send out buffer and log response
  byte ack = Wire.endTransmission();
  if ( ack == 0) {
	   Wire.requestFrom( i2cAddress , (uint8_t)1 );
	   if ( Wire.available() > 0 ) {
	   		uint8_t n = Wire.read();
	   		Serial.print(n);
	   		Wire.requestFrom(i2cAddress, (uint8_t) (n+1));
	   		if (Wire.available() > 0) {
	   			char str[n+1];
	   			Wire.read(); // already read above.
	   			for ( int i = 0; i < n; i++) {
	   				uint8_t v = Wire.read();
	   				str[i] = v;
	   				if ( v < 16) {
	   					Serial.print(" 0x0");
	   				} else {
	   					Serial.print(" 0x");
	   				}
	   				Serial.print(v,HEX);
	   			}
	   			str[n] = '\0';
	   			Serial.print(str);
	   		}
	   } else {
		   Serial.print("No len");
	   }
	   return true;
  	} else {
  		Serial.print("No Ack");
  	   return false;
  	}
 }

int BQ4050::readDataBlock( uint8_t devAddress, uint8_t regAddress, uint8_t *dataVal, uint8_t arrLen ){
	for (int i = 0; i < arrLen; i++) {
		dataVal[i] = 0;
	}
 //Add in the device address to the buffer
  Wire.beginTransmission( i2cAddress );
  //Add the one byte register address
  Wire.write( regAddress );
  //Send out buffer and log response
  byte ack = Wire.endTransmission();
  if ( ack == 0) {
	   Wire.requestFrom( i2cAddress , (uint8_t)1 );
	   if ( Wire.available() > 0 ) {
	   		uint8_t n = Wire.read();
	   		if ( n > arrLen ) {
	   			Serial.print("cmd:");
	   			Serial.print(regAddress);
	   			Serial.print(" overflow:");
	   			Serial.println(n);
	   		}
	   		Wire.requestFrom(i2cAddress, (uint8_t) (n+1));
	   		if (Wire.available() > 0) {
	   			Wire.read(); // already read above.
	   			for ( int i = 0; i < n; i++) {
	   				uint8_t v = Wire.read();
	   				if ( i < arrLen ) {
		   				dataVal[i] = v;
	   				}
	   			}
	   		}
	   		return n;
	   } else {
		   Serial.print("No len");
	   }
  	} else {
  		Serial.print("No Ack");
  	}
	return 0;
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