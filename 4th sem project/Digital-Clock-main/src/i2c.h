/**************************************************************************
   FILE          :    i2c.h
 
   PURPOSE       :    I2C library Header
 
   AUTHOR        :     K.M. Arun Kumar alias Arunkumar Murugeswaran 
 
  KNOWN BUGS     :
	
  NOTE           :   
  
  CHANGE LOGS    :
	   
 **************************************************************************/ 
#ifndef _I2C_H
#define	_I2C_H

#define INVALID_I2C_OPER_CHAR              ('\xFF')

typedef enum
{
	I2C_IDLE = 0, I2C_START, I2C_ADDR, I2C_REPEAT_START, I2C_STOP, I2C_ACK_SEQ, I2C_WRITE_SEQ, I2C_READ_SEQ
} i2c_states;

#define ACK		       0
#define NO_ACK	       1
#define I2C_READ      (1)
#define I2C_WRITE     (0) 

void I2C_Sent_Start(void);
void I2C_Sent_Stop(void);
void I2C_Sent_Restart(void);
bit I2C_Send_Dev_Addr(unsigned char Dev_Addr, const bit read_flag);
bit I2C_Write(unsigned char Data);
unsigned char I2C_Read(const bit ACK_Bit);

#endif	

/*------------------------------------------------------------------*-
  ------------------------ END OF FILE ------------------------------
-*------------------------------------------------------------------*/
