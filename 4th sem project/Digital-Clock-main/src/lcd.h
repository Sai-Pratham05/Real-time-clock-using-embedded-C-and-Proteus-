/**************************************************************************
   FILE          :    lcd.h
 
   PURPOSE       :    LCD library Header
 
   AUTHOR        :     K.M. Arun Kumar alias Arunkumar Murugeswaran 
 
  KNOWN BUGS     :
	
  NOTE           :   
  
  CHANGE LOGS    :
	   
 **************************************************************************/ 
#ifndef _LCD_H
#define	_LCD_H

void LCD_Command (char cmnd);
void LCD_Char (char char_data);
void LCD_String (char *str);
void LCD_String_xy (char row, char pos, char *str);
void LCD_Init (void);	

#endif	

/*------------------------------------------------------------------*-
  ------------------------ END OF FILE ------------------------------
-*------------------------------------------------------------------*/
