/*********************************************************************************************************
**********************************************************************************************************
							 GPIO.cpp
**********************************************************************************************************
**********************************************************************************************************
	@author		Dylan DeSantis
	@date 		3/25/2018
	@version	1.0.0
**********************************************************************************************************/
#include "GPIO.h"
#include <iostream>

/****************************************************************************************************
	Function Name:  
	Inputs: 
		1.
	Outputs:
		1. 
	Description:
		~
 ***************************************************************************************************/
int GPIO::pinMode(int pinNum, pinModes pMode)
{
// Purpose: This function sets the pin mode for the desired pin mode

	if (pinNum<10)
	{
		
		int  pn = (pinNum)*3;
		unsigned  long pM  = (unsigned long)pMode;
		void *ptr_sel1 = ptr_base + (off_t)GPFSEL0;// selection pointer for pins 0-9
		unsigned long read_result = *((unsigned long *)ptr_sel1); // read the current I/O configurations
		unsigned long desired_result = pM << pn;//shift input mask over to pin pinNum slots (found in spec sheet)
		unsigned long  bitmask =  ~(GP_MASK <<pn);// create a mask to have all 0's in the place of pin pinNum slots
		unsigned long  filtSelect = read_result & bitmask;// take the current config. and zero out the pin pinNum slot
		unsigned long newSelect = filtSelect | desired_result;//create new config. where only pin pinNum slots are changed
		*((unsigned long *)ptr_sel1) = newSelect;			
	}
	else if(9<pinNum && pinNum<20)
	{
		
		int  pn = (pinNum-10)*3;
		unsigned  long pM  = (unsigned long)pMode;
		void *ptr_sel1 = ptr_base + (off_t)GPFSEL1;// selection pointer for pins 10-19
		unsigned long read_result = *((unsigned long *)ptr_sel1); // read the current I/O configurations
		unsigned long desired_result = pM << pn;//shift input mask over to pin pinNum slots (found in spec sheet)
		unsigned long  bitmask =  ~(GP_MASK <<pn);// create a mask to have all 0's in the place of pin pinNum slots
		unsigned long  filtSelect = read_result & bitmask;// take the current config. and zero out the pin pinNum slot
		unsigned long newSelect = filtSelect | desired_result;//create new config. where only pin pinNum slots are changed
		*((unsigned long *)ptr_sel1) = newSelect;			
		
	}
	else if(19<pinNum && pinNum<30)
	{
		int  pn = (pinNum-20)*3;
		unsigned  long pM  = (unsigned long)pMode;
		void *ptr_sel1 = ptr_base + (off_t)GPFSEL2;// selection pointer for pins 20-29
		unsigned long read_result = *((unsigned long *)ptr_sel1); // read the current I/O configurations
		unsigned long desired_result = pM << pn;//shift input mask over to pin pinNum slots (found in spec sheet)
		unsigned long  bitmask =  ~(GP_MASK <<pn);// create a mask to have all 0's in the place of pin pinNum slots
		unsigned long  filtSelect = read_result & bitmask;// take the current config. and zero out the pin pinNum slot
		unsigned long newSelect = filtSelect | desired_result;//create new config. where only pin pinNum slots are changed
		*((unsigned long *)ptr_sel1) = newSelect;			
	}
	else	if(29<pinNum && pinNum<40)
	{
		int  pn = (pinNum-30)*3;
		unsigned  long pM  = (unsigned long)pMode;
		void *ptr_sel1 = ptr_base + (off_t)GPFSEL3;// selection pointer for pins 30-39
		unsigned long read_result = *((unsigned long *)ptr_sel1); // read the current I/O configurations
		unsigned long desired_result = pM << pn;//shift input mask over to pin pinNum slots (found in spec sheet)
		unsigned long  bitmask =  ~(GP_MASK <<pn);// create a mask to have all 0's in the place of pin pinNum slots
		unsigned long  filtSelect = read_result & bitmask;// take the current config. and zero out the pin pinNum slot
		unsigned long newSelect = filtSelect | desired_result;//create new config. where only pin pinNum slots are changed
		*((unsigned long *)ptr_sel1) = newSelect;			
		
	}
	else	if(39<pinNum && pinNum<50)
	{
		int  pn = (pinNum-40)*3;
		unsigned  long pM  = (unsigned long)pMode;
		void *ptr_sel1 = ptr_base + (off_t)GPFSEL4;// selection pointer for pins 40-49
		unsigned long read_result = *((unsigned long *)ptr_sel1); // read the current I/O configurations
		unsigned long desired_result = pM << pn;//shift input mask over to pin pinNum slots (found in spec sheet)
		unsigned long  bitmask =  ~(GP_MASK <<pn);// create a mask to have all 0's in the place of pin pinNum slots
		unsigned long  filtSelect = read_result & bitmask;// take the current config. and zero out the pin pinNum slot
		unsigned long newSelect = filtSelect | desired_result;//create new config. where only pin pinNum slots are changed
		*((unsigned long *)ptr_sel1) = newSelect;			
	}
	else	if(49<pinNum && pinNum<54)
	{
		int  pn = (pinNum-50)*3;
		unsigned  long pM  = (unsigned long)pMode;
		void *ptr_sel1 = ptr_base + (off_t)GPFSEL5;// selection pointer for pins 50 to 53
		unsigned long read_result = *((unsigned long *)ptr_sel1); // read the current I/O configurations
		unsigned long desired_result = pM << pn;//shift input mask over to pin pinNum slots (found in spec sheet)
		unsigned long  bitmask =  ~(GP_MASK <<pn);// create a mask to have all 0's in the place of pin pinNum slots
		unsigned long  filtSelect = read_result & bitmask;// take the current config. and zero out the pin pinNum slot
		unsigned long newSelect = filtSelect | desired_result;//create new config. where only pin pinNum slots are changed
		*((unsigned long *)ptr_sel1) = newSelect;		
	}
	else{
		printf("Pin number  %d out of range\n", pinNum);
		return -1;
	}	
	return 0;
}

/****************************************************************************************************
	Function Name:  
	Inputs: 
		1.
	Outputs:
		1. 
	Description:
		~
 ***************************************************************************************************/
int GPIO::pinModeRead(int pinNum, pinModes pMode)
{
// Purpose: This function reads the current pin mode of pinNum and compares it to the 
//		   desired pin mode pMode, returns 1 if it matches and -1 if it does not
	unsigned long desired_result, read_result;
	if (pinNum<10)
	{
		int  pn = (pinNum)*3;
		unsigned  long pM  = (unsigned long)pMode;
		void *ptr_sel1 = ptr_base + (off_t)GPFSEL0;// selection pointer for pin 0-9
		unsigned long read_result = *((unsigned long *)ptr_sel1); // read the current I/O configurations
		unsigned long desired_result = pM << pn;//shift input mask over to pin pinNum slots (found in spec sheet)		
		if(desired_result == read_result) return 1;
	}
	else if(9<pinNum && pinNum<20)
	{
		int  pn = (pinNum-10)*3;
		unsigned  long pM  = (unsigned long)pMode;
		void *ptr_sel1 = ptr_base + (off_t)GPFSEL1;// selection pointer for pin 10-19
		unsigned long read_result = *((unsigned long *)ptr_sel1); // read the current I/O configurations
		unsigned long desired_result = pM << pn;//shift input mask over to pin pinNum slots (found in spec sheet)		
		if(desired_result == read_result) return 1;
	}
	else if(19<pinNum && pinNum<30)
	{
		int  pn = (pinNum-20)*3;
		unsigned  long pM  = (unsigned long)pMode;
		void *ptr_sel1 = ptr_base + (off_t)GPFSEL2;// selection pointer for pin 20-29
		unsigned long read_result = *((unsigned long *)ptr_sel1); // read the current I/O configurations
		unsigned long desired_result = pM << pn;//shift input mask over to pin pinNum slots (found in spec sheet)		
		if(desired_result == read_result) return 1;
	}
	else	if(29<pinNum && pinNum<40)
	{
		int  pn = (pinNum-30)*3;
		unsigned  long pM  = (unsigned long)pMode;
		void *ptr_sel1 = ptr_base + (off_t)GPFSEL3;// selection pointer for pin 30-39
		unsigned long read_result = *((unsigned long *)ptr_sel1); // read the current I/O configurations
		unsigned long desired_result = pM << pn;//shift input mask over to pin pinNum slots (found in spec sheet)		
		if(desired_result == read_result) return 1;
	}
	else	if(39<pinNum && pinNum<50)
	{
		int  pn = (pinNum-40)*3;
		unsigned  long pM  = (unsigned long)pMode;
		void *ptr_sel1 = ptr_base + (off_t)GPFSEL4;// selection pointer for pin 40-49
		unsigned long read_result = *((unsigned long *)ptr_sel1); // read the current I/O configurations
		unsigned long desired_result = pM << pn;//shift input mask over to pin pinNum slots (found in spec sheet)		
		if(desired_result == read_result) return 1;
	}
	else	if(49<pinNum && pinNum<54)
	{
		int  pn = (pinNum-50)*3;
		unsigned  long pM  = (unsigned long)pMode;
		void *ptr_sel1 = ptr_base + (off_t)GPFSEL5;// selection pointer for pin 50 to 53
		unsigned long read_result = *((unsigned long *)ptr_sel1); // read the current I/O configurations
		unsigned long desired_result = pM << pn;//shift input mask over to pin pinNum slots (found in spec sheet)
		if(desired_result == read_result) return 1;
	}
	else{
		printf("Pin number  %d out of range\n", pinNum);
		return -1;
	}	
	return 0;
}

/****************************************************************************************************
	Function Name:  
	Inputs: 
		1.
	Outputs:
		1. 
	Description:
		~
 ***************************************************************************************************/
int GPIO::digitalRead(int pinNum)
{
// Purpose: This function reads the value of the pin pinNum and returns 1 if high and 
//		   0 if low, if it is neither then return -1.
	unsigned long bitHigh, pinNvalue;
	if (0<= pinNum && pinNum<32)
	{
		unsigned long bitHigh = 1 << pinNum;
		void *ptr_read = ptr_base + (off_t)GPIO_READ0;//Read pointer for pins 0 to 31
		unsigned long read_result = *((unsigned long *)ptr_read);// value of the digital pins
		unsigned long  bitmask =  (1 <<(pinNum));// create a mask to have all 1s in the place of pin pinNum
		unsigned long  pinNvalue = read_result & bitmask;// take the current config. and zero out the pin  pinNum		
		if (pinNvalue == bitHigh)  return 1;
		else if(pinNvalue == 0) return 0;
		else{
			printf("Read value of pin %d is %X pinNvalue \n", pinNum, pinNvalue);
			return -1;
		}
	}
	else if(31< pinNum && pinNum<54)
	{
		unsigned long bitHigh = 1 << pinNum;
		void *ptr_read = ptr_base + (off_t)GPIO_READ1;//Read pointer for pins 32 to 53
		unsigned long read_result = *((unsigned long *)ptr_read);// value of the digital pins
		unsigned long  bitmask =  (1 <<(pinNum-32));// create a mask to have all 1's in the place of pin pinNum
		unsigned long  pinNvalue = read_result & bitmask;// take the current config. and zero out the pin  pinNum		
		if (pinNvalue == bitHigh)  return 1;
		else	if(pinNvalue == 0) return 0;
		else{
			printf("Read value of pin %d is %X pinNvalue\n", pinNum, pinNvalue);
			return -1;
		}
	}
	else{
		printf("Pin number  %d out of range\n", pinNum);
		return -1;
	}
}

/****************************************************************************************************
	Function Name:  
	Inputs: 
		1.
	Outputs:
		1. 
	Description:
		~
 ***************************************************************************************************/
int GPIO::digitalWrite(int pinNum, outValue out_v)
{
// Purpose: This function first checks that the pin mode of the pin you wish to write to 
// 		   is set to OUTPUT. It then writes high or low to the pin.

	int status = GPIO::pinModeRead(pinNum, OUTPUT);
	if(status == -1) 
	{
		printf("Pin mode is not in output mode\n");
		return -1;
	}
	if (0<= pinNum && pinNum<32)
	{
		if(out_v == HIGH)
		{
			void *ptr_write_H =  ptr_base + (off_t)GPIO_HIGH0; // Write high pointer for pins 0 to 31
			unsigned long bitHIGH = 1 << pinNum;
			*((unsigned long *)ptr_write_H) = (unsigned long)bitHIGH;		
			return 1;
		}
		else if(out_v == LOW)
		{
			void *ptr_write_L =  ptr_base + (off_t)GPIO_LOW0; // Write low pointer for pins 0 to 31
			unsigned long bitLOW = 1 << pinNum;
			*((unsigned long *)ptr_write_L) = (unsigned long)bitLOW;
			return 1;
		}
	}
	else if(31< pinNum && pinNum<54)
	{
		if(out_v == HIGH)
		{
			void *ptr_write_H =  ptr_base + (off_t)GPIO_HIGH1;// Write high pointer for pins 32 to 53
			unsigned long bitHIGH = 1 << pinNum;
			*((unsigned long *)ptr_write_H) = (unsigned long)bitHIGH;
			return 1;
		}
		else if(out_v == LOW)
		{
			void *ptr_write_L =  ptr_base + (off_t)GPIO_LOW1; // Write low pointer for pins 32 to 53
			unsigned long bitLOW = 1 << pinNum;
			*((unsigned long *)ptr_write_L) = (unsigned long)bitLOW;
			return 1;
		}
	}
	else{
		printf("Pin number  %d out of range \n", pinNum);
		return -1;
	}
}

