

// TODO: Add pre-able/license info

// simple i2c abstraction object - use across all qwiic drivers in the pico sparkfun sdk

#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "sparkfun/qwiic_i2c.h"


// Pre launch - the Pico board didn't have defines for default I2C values
// These defines handle this situation - when a board doesn't have default defined.
// These are the values for the Pico board.

#ifndef PICO_DEFAULT_I2C_SDA
#define PICO_DEFAULT_I2C_SDA   4
#endif

#ifndef PICO_DEFAULT_I2C_SCL
#define PICO_DEFAULT_I2C_SCL   5
#endif

#ifndef PICO_DEFAULT_I2C_PORT
#define PICO_DEFAULT_I2C_PORT  i2c0
#endif


// Internal buffer size - used for writing blocks of data. This is a hack - see writeRegisterRegion
#define kWriteBlockBufferSize   64

// TODO: Hack  - this needs cleanup and refactor. 
// internal flag for bus init, which we want to do just once.

static bool _bBusIsInited = false;

//----------------------------------------------------------------------
// contructor

Qwiic_I2C::Qwiic_I2C(void){

	_setup(PICO_DEFAULT_I2C_PORT, PICO_DEFAULT_I2C_SDA, PICO_DEFAULT_I2C_SCL);
}

Qwiic_I2C::Qwiic_I2C(uint8_t sda, uint8_t scl){

	_setup(PICO_DEFAULT_I2C_PORT, sda, scl);

}
Qwiic_I2C::Qwiic_I2C(i2c_inst_t * i2cPort, uint8_t sda, uint8_t scl){

	_setup(i2cPort, sda, scl);
}

void Qwiic_I2C::_setup(i2c_inst_t * i2cPort, uint8_t sda, uint8_t scl){
	_i2c_port = i2cPort;
	_sda = sda;
	_scl = scl;

}
//----------------------------------------------------------------------
// init

bool Qwiic_I2C::init(void){

	if(!_bBusIsInited){
		//TODO: Error Handling
		i2c_init(_i2c_port, 100*1000);

		gpio_set_function(_sda, GPIO_FUNC_I2C);
    	gpio_set_function(_scl, GPIO_FUNC_I2C);

	    gpio_pull_up(_sda);
    	gpio_pull_up(_scl);

    	_bBusIsInited = true;
    }
    return true;

}

//----------------------------------------------------------------------
// I/O methods
//
// TODO: Error handling and API review
//--------------------------------------------------------------------------------------
// Return bytes recieved or PICO_ERROR_GENERIC
//
// Error = i2c error, or bytes recieved < requested.

int Qwiic_I2C::readRegisterRegion(uint8_t address, uint8_t offset, uint8_t *outputPointer , uint8_t length)
{

	int rc;
	// Where are we reading from. Keep control of the bus
	rc = i2c_write_blocking(_i2c_port, address, &offset, 1, true);
	if( rc == PICO_ERROR_GENERIC)	
		return QWIIC_ERROR_GENERIC;

	rc = i2c_read_blocking(_i2c_port, address, outputPointer, length, false);

	return rc == PICO_ERROR_GENERIC ? QWIIC_ERROR_GENERIC : rc;

}

//--------------------------------------------------------------------------------------
// Read reg with no error checking per se - helps with backward compatablity/porting old libs.

uint8_t Qwiic_I2C::readRegister(uint8_t address, uint8_t offset){


	uint8_t value;

	int rc = readRegister(address, offset, &value);

	return (rc != PICO_ERROR_GENERIC ?  value : 0);
}
//--------------------------------------------------------------------------------------
// read reg with error checking
int Qwiic_I2C::readRegister(uint8_t address, uint8_t offset, uint8_t * value)
{
	int rc;

	if(!value)
	   return QWIIC_ERROR_GENERIC;

	rc = i2c_write_blocking(_i2c_port, address, &offset, 1, true);
	if( rc == PICO_ERROR_GENERIC)
		return QWIIC_ERROR_GENERIC;

	rc = i2c_read_blocking(_i2c_port, address, value, 1, false);

	if(rc == PICO_ERROR_GENERIC)
	    return QWIIC_ERROR_GENERIC;

	return rc;
}
//--------------------------------------------------------------------------------------
int Qwiic_I2C::readRegisterInt16( uint8_t address,  uint8_t offset, uint16_t *value )
{
	uint8_t myBuffer[2];
	int rc;

	if(!value)
		return QWIIC_ERROR_GENERIC;

	rc = readRegisterRegion(address, offset, myBuffer, 2);  //Does memory transfer

	if(rc == PICO_ERROR_GENERIC || rc < 2)
		return QWIIC_ERROR_GENERIC;

	*value = (int16_t)myBuffer[0] | int16_t(myBuffer[1] << 8);
	
	return rc;
}

//----------------------------------------------------------------------------------
// write methods
//----------------------------------------------------------------------------------
// just write nothing to an register 
void Qwiic_I2C::write(uint8_t address, uint8_t offset){

	i2c_write_blocking(_i2c_port, address,  &offset, 1, false);	
}

bool Qwiic_I2C::writeRegister(uint8_t address, uint8_t offset, uint8_t dataToWrite)
{

	uint8_t myBuffer[2] = {offset, dataToWrite};

	int rc;
    rc = i2c_write_blocking(_i2c_port, address, myBuffer, 2, false);

    return (rc != PICO_ERROR_GENERIC);
}


//----------------------------------------------------------------------------------
int Qwiic_I2C::writeRegisterRegion(uint8_t address, uint8_t offset, uint8_t *data, uint8_t length)
{
	int rc;

	uint8_t buffer[length+1];	

	buffer[0] = offset;
	memcpy(buffer+1, data, length);


	rc = i2c_write_blocking(_i2c_port, address, buffer, length+1, false);	

	if( rc == PICO_ERROR_GENERIC)	
		return QWIIC_ERROR_GENERIC;
	
	return rc;

}
//----------------------------------------------------------------------------------
// Option if the i2c system is limited - ....not sure if needed ..
//
// Breakup the data into small chunks and send to rp2
//
int Qwiic_I2C::writeRegisterRegionChunked(uint8_t address, uint8_t offset, uint8_t *data, uint8_t length)
{
	int rc;

	const int CHUNK_SIZE = 32;

	uint8_t buffer[CHUNK_SIZE+1];	

	int iCurrentIndex = 0;
	int nLeft = length;
	int nToWrite;

	buffer[0] = offset;

	while(nLeft > 0){

		nToWrite = nLeft > CHUNK_SIZE ?  CHUNK_SIZE : nLeft;

		memcpy(buffer+1, data+iCurrentIndex, nToWrite);

		nLeft -= nToWrite;
		iCurrentIndex += nToWrite;

		rc = i2c_write_blocking(_i2c_port, address, buffer, nToWrite+1, nLeft <=0);	

		if( rc == PICO_ERROR_GENERIC)	
			return QWIIC_ERROR_GENERIC;
	}
	
	return length;

}
//----------------------------------------------------------------------------------
bool Qwiic_I2C::writeRegisterInt16(uint8_t address, uint8_t offset, uint16_t dataToWrite){

	uint8_t myBuffer[3] = {offset, (uint8_t)(dataToWrite & 0xFF), (uint8_t)(dataToWrite >> 8 & 0xFF) };

	int rc = i2c_write_blocking(_i2c_port, address, myBuffer, 3, false);

	return (rc != PICO_ERROR_GENERIC);

}
//----------------------------------------------------------------------------------
bool Qwiic_I2C::pingDevice(uint8_t address){

	return i2c_write_blocking(_i2c_port, address, NULL, 0, false) == PICO_ERROR_GENERIC ? false : true;
}