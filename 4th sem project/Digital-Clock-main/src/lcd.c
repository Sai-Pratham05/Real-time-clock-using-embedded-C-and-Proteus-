/**************************************************************************
   FILE          :    lcd.c
 
   PURPOSE       :    LCD library Header
 
   AUTHOR        :     K.M. Arun Kumar alias Arunkumar Murugeswaran 
 
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

Func ID        :    
-*------------------------------------------------------------*/
void LCD_Command (char cmnd)   
{
     LCD_Port =(LCD_Port & 0x0F) | (cmnd & 0xF0); /* sending upper nibble */
     rs=0;	 													  /* command reg. */
     en=1; 
		Delay_in_Milli_Sec(1);
			en=0;
		Delay_in_Milli_Sec(2);
	
	  LCD_Port = (LCD_Port & 0x0F) | (cmnd << 4);  /* sending lower nibble */
		 en=1; 									/* enable pulse */
		Delay_in_Milli_Sec(1);
		en=0;
		Delay_in_Milli_Sec(5);
}

/*------------------------------------------------------------*
FUNCTION NAME  : 

DESCRIPTION    : 
								
INPUT          : none

OUTPUT         : 

NOTE           : 

Func ID        :    
-*------------------------------------------------------------*/
void LCD_Char (char char_data)							/* LCD data write function */
{
    LCD_Port =(LCD_Port & 0x0F) | (char_data & 0xF0); /* sending upper nibble */    
    rs=1;									/*Data reg.*/
    en=1;   				
		Delay_in_Milli_Sec(1);
		en=0;
		Delay_in_Milli_Sec(2);

	  LCD_Port = (LCD_Port & 0x0F) | (char_data << 4);  /* sending lower nibble */
		en=1; 									/* enable pulse */
		Delay_in_Milli_Sec(1);
		en=0;
		Delay_in_Milli_Sec(5);

}

/*------------------------------------------------------------*
FUNCTION NAME  : 

DESCRIPTION    : 
								
INPUT          : none

OUTPUT         : 

NOTE           : 

Func ID        :    
-*------------------------------------------------------------*/
void LCD_String (char *str)							/* Send string to LCD function */
{
	int i;
	for(i=0;str[i]!=0;i++)							/* Send each char of string till the NULL */
	{
		LCD_Char (str[i]);							/* Call LCD data write */
	}
}

/*------------------------------------------------------------*
FUNCTION NAME  : 

DESCRIPTION    : 
								
INPUT          : none

OUTPUT         : 

NOTE           : 

Func ID        :    
-*------------------------------------------------------------*/
void LCD_String_xy (char row, char pos, char *str)	/* Send string to LCD function */
{
	if (row == 0)
	   LCD_Command((pos & 0x3F)|0x80);					/* Command of first row and required position<20 */
	else 
	   LCD_Command((pos & 0x3F)|0xC0);					/* Command of first row and required position<20 */
	LCD_String(str);								/* Call LCD string function */
}

/*------------------------------------------------------------*
FUNCTION NAME  : 

DESCRIPTION    : 
								
INPUT          : none

OUTPUT         : 

NOTE           : 

Func ID        :    
-*------------------------------------------------------------*/
void LCD_Init (void)					/* LCD Initialize function */
{
	
	Delay_in_Milli_Sec(20);					/* LCD Power ON Initialization time >15ms */
	LCD_Command (0x02);				/* 4bit mode */
	LCD_Command (0x28);				/* Initialization of 20X2 LCD in 4bit mode */
	LCD_Command (0x0C);				/* Display ON Cursor OFF */
	LCD_Command (0x06);				/* Auto Increment cursor */
	LCD_Command (0x01);				/* clear display */
	LCD_Command (0x80);				/* cursor at home position */
}

/*-------------------------------------------------------------------
  ------------------------ END OF FILE ------------------------------
-------------------------------------------------------------------*/
