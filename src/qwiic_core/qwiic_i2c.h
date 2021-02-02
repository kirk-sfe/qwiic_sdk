
// Simple qwiic object for i2c and other abstraction

#ifndef __SPARKFUN_QWIIC_I2C_H__
#define __SPARKFUN_QWIIC_I2C_H__

// Include the i2c info from pico sdk
#include "hardware/i2c.h"

// Simple error codes for now
#define QWIIC_ERROR_GENERIC   -1
#define QWIIC_ERROR_OKAY       0

// Define the class
class Qwiic_I2C {
	
  public:
    
    // constructors
    Qwiic_I2C(void);
    Qwiic_I2C(uint8_t sda, uint8_t scl);    
    Qwiic_I2C(i2c_inst_t *i2cPort, uint8_t sda, uint8_t scl);

    // Initialize/startup
    bool init(void);  // true - success, false - error


    // I/O methods
    int readRegisterRegion(uint8_t address, uint8_t, uint8_t*, uint8_t );

	//readRegister reads one register - error checking and blind read methods
    int readRegister(uint8_t address, uint8_t, uint8_t *); // returns error value

    uint8_t readRegister(uint8_t address, uint8_t);
    
    //Reads two regs, LSByte then MSByte order, and concatenates them
	//Used for two-byte reads
	int readRegisterInt16(uint8_t address, uint8_t offset, uint16_t * );
	
    void write(uint8_t address, uint8_t);  // write nothing to a reg.
	//Writes a byte;
    bool writeRegister(uint8_t address, uint8_t, uint8_t);

    int writeRegisterRegion(uint8_t address, uint8_t offset, uint8_t *data, uint8_t length);
    int writeRegisterRegionChunked(uint8_t address, uint8_t offset, uint8_t *data, uint8_t length);    

    bool writeRegisterInt16(uint8_t address, uint8_t offset, uint16_t dataToWrite);

    bool pingDevice(uint8_t address);

private:
    void _setup(i2c_inst_t *i2cPort, uint8_t sda, uint8_t scl);
	i2c_inst_t * _i2c_port;

	uint8_t      _sda;
	uint8_t 	 _scl;

};

#endif
