/**************************************************************************
   FILE          :    port.h
 
   PURPOSE       :    port header - define port and its pin assignment.
 
   AUTHOR        :    K.M. Arun Kumar alias Arunkumar Murugeswaran
 
  KNOWN BUGS     :
	
  NOTE           :    port header - user interface to external device, such as LED, Switch,
 	connection to other microcontroller via RS232, USB, etc. 
 To avoid this header file to be included more than once, conditional directive is used  
	
  CHANGE LOGS     :  
	   
 **************************************************************************/
#ifndef _PORT_H
#define _PORT_H

 
/* Pins 3.0(Rx) and 3.1 (Tx) are used by on chip UART of uC 8051. On Chip UART used for RS-232 interface.
/* -------- LED Interface --------- */
sbit ERROR_LED                = P1^7;

/* -------- LED Interface --------- */
sbit SET_RTC_SW               = P1^1;
sbit INC_VALUE_SW             = P1^2;
sbit NEXT_DATA_SW             = P1^3;

/* -------- LCD Interface --------- */
#define LCD_Port                   P2				 				
sbit rs                     = P2^3;        					    /* Register select pin */
sbit en                     = P2^2;        				    	 /* Enable pin */  

/* -------  DS1307 Interface ------- */
sbit SDA    = P0^2; 
sbit SCL    = P0^1;   

#endif 

/*-------------------------------------------------------------------
  ---- END OF FILE -------------------------------------------------
-------------------------------------------------------------------*/
