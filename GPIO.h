#ifndef GPIOC_H
#define GPIOC_H
/*********************************************************************************************************
**********************************************************************************************************
							   GPIO.h
**********************************************************************************************************
**********************************************************************************************************
	_filename: GPIO.h
	_description:This Class maps the GPIO memory for the Raspberry Pi 3 B. It provides access to 
			     the GPIO pins and their registers. Simple functionality is implemented, with the 
			     ability to add more advanced functions.
				
***************************************References******************************************************
https://www.raspberrypi.org/app/uploads/2012/02/BCM2835-ARM-Peripherals.pdf  (page 91)
************************************************************************************************************
	@author		Dylan DeSantis
	@date 		3/25/2018
	@version	1.0.0
************************************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <ctype.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/mman.h>

#define PERPH_BASE 0x3F000000
#define GPIO_BASE 0x200000
#define GPIO_READ0 0x0034
#define GPIO_READ1 0x0038
#define GPIO_HIGH0 0x001C
#define GPIO_HIGH1 0x0020
#define GPIO_LOW0 0x0028
#define GPIO_LOW1 0x002C
#define MAP_SIZE 4096UL
#define MAP_MASK (MAP_SIZE-1)
#define MEM_PATH "/dev/mem"
#define GPIO_PATH "/dev/gpiomem"		
#define GP_MASK 0x7

#define GPFSEL0	0x0000
#define GPFSEL1 0x0004
#define GPFSEL2	0x0008
#define GPFSEL3	0x000C
#define GPFSEL4	0x0010
#define GPFSEL5	0x0014

typedef enum{ HIGH = 1, LOW = 0} outValue ;
typedef enum {
	INPUT =  0x0,
	OUTPUT = 0x1,
	ALT0 = 0x4,
	ALT1 = 0x5,
	ALT2 = 0x6,
	ALT3 = 0x7,
	ALT4 = 0x3,
	ALT5 = 0x2
	} pinModes;

class GPIO
{
public:
	void *ptr_base;
	int memfd;
public:
	int  pinMode(int, pinModes);
	int  pinModeRead(int, pinModes);
	int digitalRead(int);
	int digitalWrite(int, outValue);
	GPIO()
	{
		off_t gpio_base = ((off_t)(PERPH_BASE+GPIO_BASE ));// base address for GPIO
		memfd = open(MEM_PATH, O_RDWR | O_SYNC);// open driver
		//memfd = open(GPIO_PATH, O_RDWR | O_SYNC);// open driver
		if (memfd<0)  printf("Cannot open");
		ptr_base = mmap(NULL,  MAP_SIZE, (PROT_READ | PROT_WRITE), MAP_SHARED, memfd, gpio_base);// Mapping 
	}
	GPIO(int fd, void* ptr):
		ptr_base(ptr),
		memfd(fd)
	{
		off_t gpio_base = ((off_t)(PERPH_BASE+GPIO_BASE ));// base address for GPIO
	}
	~GPIO()
	{
		munmap(ptr_base,MAP_SIZE);
		close(memfd);
	}	
};
#endif


