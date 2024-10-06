/**************************************************************************
   FILE          :    i2c.c
 
   PURPOSE       :   I2C Library
 
   AUTHOR        :    K.M. Arun Kumar alias Arunkumar Murugeswaran 
 
  KNOWN BUGS     :
	
  NOTE           :   
  
  CHANGE LOGS    :
	   
 **************************************************************************/
#include "main.h"


/*------------------------------------------------------------*
FUNCTION NAME  : 

DESCRIPTION    : 
								
INPUT          : none

OUTPUT         : 

NOTE           : 

Func ID        : 21   
-*------------------------------------------------------------*/
void I2C_Sent_Start(void)
{
    SDA = 1;
	SCL = 1;
	_nop_(); _nop_();
	SDA = 0;
	_nop_();_nop_();
	SCL = 0;
	_nop_();_nop_();	  
}
	
/*------------------------------------------------------------*
FUNCTION NAME  : 

DESCRIPTION    :
								
INPUT          : none

OUTPUT         : 

NOTE           : 

Func ID        : 21   
-*------------------------------------------------------------*/
void I2C_Sent_Stop(void)
{
	SDA = 0;	    	
	_nop_();_nop_();
	SCL = 1;
	_nop_();_nop_();
	SDA = 1;	
}

/*------------------------------------------------------------*
FUNCTION NAME  : 

DESCRIPTION    :
								
INPUT          : none

OUTPUT         : 

NOTE           : 

Func ID        : 21   
-*------------------------------------------------------------*/
// Function Purpose: I2C_ReStart sends start bit sequence
void I2C_Sent_Restart(void)
{
	
	SDA = 1;	    	
	_nop_();_nop_();
	SCL = 1;
	_nop_();_nop_();
	SDA = 0;
        _nop_(); _nop_();
        SCL = 0;
	_nop_(); _nop_();
}

/*------------------------------------------------------------*
FUNCTION NAME  : 

DESCRIPTION    :
								
INPUT          : none

OUTPUT         : 

NOTE           : 

Func ID        : 21   
-*------------------------------------------------------------*/
bit I2C_Send_Dev_Addr(unsigned char Dev_Addr, bit r_w_flag)
{    
   unsigned int i;
   unsigned char Data = Dev_Addr;
   bit ack_flag = ACK;
   
   if(r_w_flag == I2C_READ)
      Data	|= 0x01;	 
   for (i=0;i<8;i++)
   {
	   SDA = (Data & 0x80) ? 1 : 0;
	   SCL=1; 
       Data<<=1;
       SCL =0; 		 
   }
   while(SDA != 0);
   SCL = 1; 
   _nop_(); _nop_();
   SCL = 0;   
   _nop_(); _nop_();
   return SDA;    
}

/*------------------------------------------------------------*
FUNCTION NAME  : 

DESCRIPTION    :
								
INPUT          : none

OUTPUT         : 

NOTE           : 

Func ID        : 21   
-*------------------------------------------------------------*/
bit I2C_Write(unsigned char Data)
{    
   unsigned int i;
   bit ack_flag = ACK;

   for (i=0;i<8;i++)
   {
         SDA = (Data & 0x80) ? 1 : 0;
	     SCL=1;
         Data<<=1;	
		 SCL =0;
   }
   while(SDA != 0);
   SCL = 1; 
  _nop_();_nop_();
	 SCL = 0;   
   _nop_(); _nop_();
   return SDA;   
}

/*------------------------------------------------------------*
FUNCTION NAME  : 

DESCRIPTION    :
								
INPUT          : none

OUTPUT         : 

NOTE           : 

Func ID        : 21   
-*------------------------------------------------------------*/
unsigned char I2C_Read(const bit ACK_Bit)
{
    unsigned char Data = 0x00; 
	unsigned int i;
	
	SDA = 1;	
	for (i=0;i<8;i++)
	{
		SCL   = 1;		
		Data<<= 1;
		Data  = (Data | SDA);		
		SCL   = 0;
		_nop_();
	}
    SDA = ACK_Bit; // Send ACK	
	_nop_();_nop_();
	SCL = 1;		
	_nop_();_nop_();
	SCL = 0;
  	return Data;
}

 /*------------------------------------------------------------------*-
  ------------------------ END OF FILE ------------------------------
-*------------------------------------------------------------------*/
