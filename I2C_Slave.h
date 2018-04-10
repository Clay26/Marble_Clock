/*
_filename: I2C_Slave.h
_description: Declaration file for a I2C slave class.
//=====================================================================
// ------------------------------------ I2C Slave Class Declaration --------------------------------------------------
// ____________________________________________________________________________
// xxxx This class defines a general layout for slave devices connected to the 
// xxxx master. Use this class a parent class for specific slave devices with which
// xxxx you communicate via I2C
//=====================================================================
@author Dylan DeSantis
@version 1.0.0
*/
#ifndef I2C_SLAVE_H
#define I2C_SLAVE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <ctype.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <chrono>
#include <ctime>
#include <sys/stat.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <sys/ioctl.h>
#include <deque>

//=====================================================================
// ------------------------------------ General Masks and File Paths --------------------------------------------------
//=====================================================================		
#define I2C1_PATH "/dev/i2c-1"	

//Which driver is being used for the i2c
typedef enum{
	DEV0 = 0,
	DEV1 = 1
	} i2cDevice;

class I2C_Slave
{
private:
	unsigned char slv_addr; // slave address of the device 
	int i2c_fd; //file descriptor for the open i2c driver 
public:
//=====================================================================
// --------------------------------------MEM: Buffers to Read/Write  --------------------------------------------------
//=====================================================================
	std::deque<unsigned char>  stream_in;//used to receive and hold data of any length
	std::deque<unsigned char> stream_out;// used to send  and hold data of any length
	std::deque<unsigned char> register2read;//sequence of registers to read from /paired with stream_in
	std::deque<unsigned char> register2write;//sequence of registers to write to/ paired with stream_out
//=====================================================================
// -------------------------------------- METH: Open/closing and utilities -------------------------------------------
//=====================================================================
	void i2cBegin(); 
	int i2cEnd();
	void i2cDelay(int);
	void i2cPutBlkCmds(unsigned char*, unsigned char*, int, int);
	void i2cClearQueues();
//=====================================================================
// -------------------------------------- METH: Read/ Write  ----------------------------------------------------------------
//=====================================================================
	unsigned char i2cRead8(unsigned char); 
	int i2cWrite8(unsigned char, unsigned char);
	void i2cStreamInSeq();
	void i2cStreamOutSeq();
	void i2cStreamInBlock(unsigned char, unsigned char,unsigned char,unsigned char); 
	 void i2cStreamOutBlock(unsigned char, unsigned char,unsigned char);
   void s();
//=====================================================================
// -------------------------------------- METH: Constructor/ Destructor----------------------------------------------
//=====================================================================
	I2C_Slave(unsigned char slv_dev_addr, int fd):
		slv_addr (slv_dev_addr),
		i2c_fd(fd)
	{
		
	}
	~I2C_Slave()
	{
	
	}	
};
#endif



