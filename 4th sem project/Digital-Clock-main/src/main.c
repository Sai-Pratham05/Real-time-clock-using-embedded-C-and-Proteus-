/* ********************************************************************
FILE                   : main.c

PROGRAM DESCRIPTION    : Digital Clock using Interface I2C with RTC DS1307. 
                         SET_RTC_SW is used to start set date and time.
                         INC_VALUE_SW	is used to increment the selected RTC data.
                         NEXT_DATA_SW is used to select the RTC data.
                         1: After interfacing with DS1307 using I2C, after read RTC data, 
												  then RTC data is converted BCD to Decimal and displayed in LCD.
                         2: When SET_RTC_SW is pressed, its starts the process the set date and time, 
												 and goes to first select Year to set. By each press of INC_VALUE_SW, 
												 Year is increment with roll back.
												 3: When NEXT_DATA_SW is pressed, year is set and selects month to set. 
												 By each press of INC_VALUE_SW, month is increment with roll back.
												 4: When NEXT_DATA_SW is pressed, month is set and selects date to set. 
												 By each press of INC_VALUE_SW, date is increment with roll back.
												 5: Day in numeric is calcuated from date, month and year.
												 6: When NEXT_DATA_SW is pressed, date is set and selects hour to set. 
												 By each press of INC_VALUE_SW, hour is increment with roll back.
												 7: When NEXT_DATA_SW is pressed, hour is set and selects minute to set. 
												 By each press of INC_VALUE_SW, minute is increment with roll back.
                         8: When NEXT_DATA_SW is pressed, minute is set and selects seconds to set. 
												 By each press of INC_VALUE_SW, seconds is increment with roll back. 	
												 9: When NEXT_DATA_SW is pressed, seconds is set and converts selected date and time 
												    with Decimal to BCD and then configures date and time of DS1307 RTC. Then goto Step 1.
	 
AUTHOR                : K.M. Arun Kumar alias Arunkumar Murugeswaran
	 
KNOWN BUGS            : 

NOTE                  :  https://circuitdigest.com/microcontroller-projects/digital-clock-using-8051-microcontroller	   							
                                    
CHANGE LOGS           : 

*****************************************************************************/

#include "main.h"
#include <stdio.h>

typedef enum 
{
	FSM_SET_DATA = 0, FSM_READ_RTC, FSM_DISP_RTC, FSM_SET_DATE = 0, FSM_SET_MONTH, FSM_SET_YEAR, FSM_SET_DAY, FSM_SET_HOUR, FSM_SET_MINUTE, 
	FSM_SET_SECOND, FSM_RTC_DEC_TO_BCD_CONV, FSM_RTC_CONF, LCD_RTC_DISP_ALL_DATA = 0, LCD_RTC_DISP_DATE_DATA, LCD_RTC_DISP_MONTH_DATA,
	LCD_RTC_DISP_YEAR_DATA, LCD_RTC_DISP_HOUR_DATA, LCD_RTC_DISP_MINUTE_DATA, LCD_RTC_DISP_SECOND_DATA, LCD_RTC_DISP_DAY_DATA 	
} fsm_system_t;

fsm_system_t fsm_system;

typedef struct
{
  unsigned int  cur_fsm_system         : 1;
	unsigned int  set_sw_enable_flag     : 1;
	unsigned int  inc_sw_enable_flag     : 1;
	unsigned int  next_sw_enable_flag    : 1;
	unsigned int  cur_fsm_set_rtc_data   : 4;
} system_ctrl_status_t;
system_ctrl_status_t system_ctrl_status;
	
unsigned int date = 21, month = 5, hour = 0, minute = 0, seconds = 0, day = 1;
unsigned long int year = 23; 

static unsigned int Decimal_To_BCD (unsigned int decimalByte);
static unsigned int BCD_To_Decimal(unsigned int bcdByte);
static void LCD_Show_Time(const unsigned char display_data);
static unsigned int Day_of_Week(const unsigned date, const unsigned int month, unsigned long int year );
static void Set_Time();

/*------------------------------------------------------------*-
FUNCTION NAME  : main

DESCRIPTION    :          
								
INPUT          : none

OUTPUT         : 

NOTE           : 
-*------------------------------------------------------------*/

void main(void)
{
	unsigned int sw_debounce;
	
	 /* ========= begin : USER CODE INITIALIZE ========== */
	 ERROR_LED = 0;         //output for MCU
	 SET_RTC_SW = 1;
	 INC_VALUE_SW = 1;
	 NEXT_DATA_SW = 1;
	 UART_Init();
	 LCD_Init();
	 LCD_String_xy(0, 0, "DATE:");
	 LCD_String_xy(1, 0, "TIME:");
	 system_ctrl_status.cur_fsm_system = FSM_READ_RTC;
	 system_ctrl_status.set_sw_enable_flag = STATE_YES;
	 system_ctrl_status.inc_sw_enable_flag = STATE_NO;
	 system_ctrl_status.next_sw_enable_flag = STATE_NO;
     /* ========= end : USER CODE INITIALIZE ========== */
	  
     while(1) // Super Loop
     {
		 if(system_ctrl_status.set_sw_enable_flag == STATE_YES && SET_RTC_SW == 0)  
		 {
			 sw_debounce = SW_DEBOUNCE_COUNT;
		     Delay_in_Milli_Sec(SW_DEBOUNCE_TIMEOUT);
			 while(SET_RTC_SW == 0 && --sw_debounce);
	         system_ctrl_status.set_sw_enable_flag = STATE_NO; 
			 system_ctrl_status.inc_sw_enable_flag = STATE_YES;
			 system_ctrl_status.next_sw_enable_flag = STATE_YES;			
			 LCD_Command(0x0C);
			 UART_Tx_Str("\r INFO: -> Set Data"); 		
             system_ctrl_status.cur_fsm_system = FSM_SET_DATA; 
             system_ctrl_status.cur_fsm_set_rtc_data = FSM_SET_YEAR;
             LCD_String_xy(0, 17, "---");			 
		 }
       switch(system_ctrl_status.cur_fsm_system)                                
	     {		 
			 case FSM_SET_DATA:				
                Set_Time();				    
			 break;
             case FSM_READ_RTC:
			    I2C_Sent_Start();
                I2C_Write(0xD0); 
                I2C_Write(0x00); 
                I2C_Sent_Start(); 
                I2C_Write(0xD1); 
                seconds = BCD_To_Decimal(I2C_Read(0));
                minute = BCD_To_Decimal(I2C_Read(0));
                hour = BCD_To_Decimal(I2C_Read(0));
                day = BCD_To_Decimal(I2C_Read(0));				
                date = BCD_To_Decimal(I2C_Read(0));
                month = BCD_To_Decimal(I2C_Read(0));
                year = BCD_To_Decimal(I2C_Read(1));
                I2C_Sent_Stop();
				LCD_Show_Time(LCD_RTC_DISP_ALL_DATA); 
                Delay_in_Milli_Sec(1);                  
             break;			   
		 }
	 }		 
	 return; 
}

/*------------------------------------------------------------*-
FUNCTION NAME  : 

DESCRIPTION    :   date and time set function
								
INPUT          : 

OUTPUT         : 

NOTE           : 
-*------------------------------------------------------------*/
void Set_Time()                                            //
{
	char is_leap_year, disp_str[DISP_STR_NUM_CHARS];
	    
    switch(system_ctrl_status.cur_fsm_set_rtc_data)                                
	{
			case FSM_SET_YEAR:
			    if(system_ctrl_status.inc_sw_enable_flag == STATE_YES && INC_VALUE_SW == 0)
                {
				   Delay_in_Milli_Sec(SW_DEBOUNCE_TIMEOUT);
				   while(INC_VALUE_SW == 0);								
				   ++year;
				   if(year >= 41)
					   year = 23;				   
			   }			   
               if(system_ctrl_status.next_sw_enable_flag == STATE_YES && NEXT_DATA_SW == 0)
			   {
				   Delay_in_Milli_Sec(SW_DEBOUNCE_TIMEOUT);
				   while(NEXT_DATA_SW == 0);
				   UART_Tx_Str("\r INPUT : Set Year = ");
				   sprintf(disp_str, "%04lu", year + 2000UL);
				   UART_Tx_Str(disp_str);
				   UART_Tx_Str("\r INFO: -> Set Month");
				   system_ctrl_status.cur_fsm_set_rtc_data = FSM_SET_MONTH;		
			   }
			   LCD_Show_Time(LCD_RTC_DISP_YEAR_DATA);
			break;
			case FSM_SET_MONTH:
			   if(system_ctrl_status.inc_sw_enable_flag == STATE_YES && INC_VALUE_SW == 0)
               {
				   Delay_in_Milli_Sec(SW_DEBOUNCE_TIMEOUT);
				   while(INC_VALUE_SW == 0);
				   ++month;
				   if(month >= 13)
					   month = 1;	
			   }
			   if(system_ctrl_status.next_sw_enable_flag == STATE_YES && NEXT_DATA_SW == 0)
			   {
				   Delay_in_Milli_Sec(SW_DEBOUNCE_TIMEOUT);
				   while(NEXT_DATA_SW == 0);
				   UART_Tx_Str("\r INPUT: Set Month = ");
				   sprintf(disp_str, "%02u", month);
				   UART_Tx_Str(disp_str);
				   UART_Tx_Str("\r INFO: -> Set Date");
				   system_ctrl_status.cur_fsm_set_rtc_data = FSM_SET_DATE;		
			   }
			   LCD_Show_Time(LCD_RTC_DISP_MONTH_DATA);
			break;
            case FSM_SET_DATE:
               if(system_ctrl_status.inc_sw_enable_flag == STATE_YES && INC_VALUE_SW == 0)
               {
				   Delay_in_Milli_Sec(SW_DEBOUNCE_TIMEOUT);
				   while(INC_VALUE_SW == 0);
				   is_leap_year = year % 4;
				   ++date;
				   if((month == 2 && ((is_leap_year != 0 && date >= 29) || (is_leap_year == 0 && date >= 30 ))) || 
				     ((date >= 31) && (month == 4 || month == 6 || month == 9 || month == 11)) || date >= 32)
					   date = 1;				   
			   }
			   if(system_ctrl_status.next_sw_enable_flag == STATE_YES && NEXT_DATA_SW == 0)
			   {
				   Delay_in_Milli_Sec(SW_DEBOUNCE_TIMEOUT);
				   while(NEXT_DATA_SW == 0);
				   UART_Tx_Str("\r INPUT: Set Date = ");
				   sprintf(disp_str, "%02u", date);
				   UART_Tx_Str(disp_str);
				   UART_Tx_Str("\r INFO: -> Set Day");
				   system_ctrl_status.cur_fsm_set_rtc_data = FSM_SET_DAY;		
			   }
			   LCD_Show_Time(LCD_RTC_DISP_DATE_DATA);
            break;
			case FSM_SET_DAY:
			   day = Day_of_Week(date, month, year);
			   ++day;  // for DS1307 RTC day
			   LCD_Show_Time(LCD_RTC_DISP_DAY_DATA);
			   UART_Tx_Str("\r Calc: Day = ");
			   sprintf(disp_str, "%02u", day);
			   UART_Tx_Str(disp_str);
			   UART_Tx_Str("\r INFO: -> Set Hour");
			   system_ctrl_status.cur_fsm_set_rtc_data = FSM_SET_HOUR;	
			break;
            case FSM_SET_HOUR:			    
			   if(system_ctrl_status.inc_sw_enable_flag == STATE_YES && INC_VALUE_SW == 0)
               {
				   Delay_in_Milli_Sec(SW_DEBOUNCE_TIMEOUT);
				   while(INC_VALUE_SW == 0);
				   ++hour;
				   if(hour >= 24)
					   hour = 0;                    				   
			   }
			   if(system_ctrl_status.next_sw_enable_flag == STATE_YES && NEXT_DATA_SW == 0)
			   {
				   Delay_in_Milli_Sec(SW_DEBOUNCE_TIMEOUT);
				   while(NEXT_DATA_SW == 0);
				   UART_Tx_Str("\r INPUT : Hour = ");
			       sprintf(disp_str, "%02u", hour);
			       UART_Tx_Str(disp_str);
			       UART_Tx_Str("\r INFO: -> Set Minute");
				   system_ctrl_status.cur_fsm_set_rtc_data = FSM_SET_MINUTE;		
			   }
			   LCD_Show_Time(LCD_RTC_DISP_HOUR_DATA); 
            break;
            case FSM_SET_MINUTE:
			   if(system_ctrl_status.inc_sw_enable_flag == STATE_YES && INC_VALUE_SW == 0)
               {
				   Delay_in_Milli_Sec(SW_DEBOUNCE_TIMEOUT);
				   while(INC_VALUE_SW == 0);
				   ++minute;
				   if(minute >= 60)
					   minute = 0;				  
			   }
			   if(system_ctrl_status.next_sw_enable_flag == STATE_YES && NEXT_DATA_SW == 0)
			   {
				   Delay_in_Milli_Sec(SW_DEBOUNCE_TIMEOUT);
				   while(NEXT_DATA_SW == 0);
				   UART_Tx_Str("\r INPUT : Minute = ");
			       sprintf(disp_str, "%02u", minute);
			       UART_Tx_Str(disp_str);
			       UART_Tx_Str("\r INFO: -> Set Seconds");
				   system_ctrl_status.cur_fsm_set_rtc_data = FSM_SET_SECOND;		
			   }
               LCD_Show_Time(LCD_RTC_DISP_MINUTE_DATA);
            break; 			
      		case FSM_SET_SECOND:
			   if(system_ctrl_status.inc_sw_enable_flag == STATE_YES && INC_VALUE_SW == 0)
               {
				   Delay_in_Milli_Sec(SW_DEBOUNCE_TIMEOUT);
				   while(INC_VALUE_SW == 0);
				   ++seconds;
				   if(seconds >= 60)
					   seconds = 0;				   
			   }
			   if(system_ctrl_status.next_sw_enable_flag == STATE_YES && NEXT_DATA_SW == 0)
			   {
				   Delay_in_Milli_Sec(SW_DEBOUNCE_TIMEOUT);
				   while(NEXT_DATA_SW == 0);
				   UART_Tx_Str("\r INPUT : Seconds = ");
			       sprintf(disp_str, "%02u", seconds);
			       UART_Tx_Str(disp_str);
			       UART_Tx_Str("\r INFO: -> Decimal to BCD Conv");
				   system_ctrl_status.cur_fsm_set_rtc_data = FSM_RTC_DEC_TO_BCD_CONV;		
			   }
			   LCD_Show_Time(LCD_RTC_DISP_SECOND_DATA);
            break;
 			case FSM_RTC_DEC_TO_BCD_CONV:
			    minute = Decimal_To_BCD(minute);
                seconds = Decimal_To_BCD(seconds);
                hour = Decimal_To_BCD(hour);
                year = Decimal_To_BCD(year);
                month = Decimal_To_BCD(month);
                date = Decimal_To_BCD(date); 
				UART_Tx_Str("\r INFO: -> RTC Conf");
				system_ctrl_status.cur_fsm_set_rtc_data = FSM_RTC_CONF;
			 break;
			 case FSM_RTC_CONF:
			   I2C_Sent_Start();
               I2C_Write(0xD0);
               I2C_Write(0x00);
               I2C_Write(seconds);
               I2C_Write(minute);
               I2C_Write(hour);
               I2C_Write(day);
               I2C_Write(date);
               I2C_Write(month);
               I2C_Write(year);  
               I2C_Write(0x80); 
               I2C_Sent_Stop(); 
			   LCD_Command(0x01);
			   LCD_String_xy(0, 0, "DATE:");
	           LCD_String_xy(1, 0, "TIME:");
               LCD_Command(0x0C);
			   system_ctrl_status.inc_sw_enable_flag = STATE_NO;
			   system_ctrl_status.next_sw_enable_flag = STATE_NO;
			   system_ctrl_status.set_sw_enable_flag = STATE_YES;
			   UART_Tx_Str("\r INFO: -> Read RTC");
			   system_ctrl_status.cur_fsm_system = FSM_READ_RTC;
               return;			   
          break;
	  }
}  
/*------------------------------------------------------------*-
FUNCTION NAME  : 

DESCRIPTION    : function to display time/date/day on LCD         
								
INPUT          : 

OUTPUT         : 

NOTE           : 
-*------------------------------------------------------------*/
void LCD_Show_Time(const unsigned char display_data)                            
{
  char var[DISP_STR_NUM_CHARS];
  
  if(display_data == LCD_RTC_DISP_ALL_DATA || display_data == LCD_RTC_DISP_DATE_DATA)
  {
	  sprintf(var,"%02u",date);
	  if(display_data == LCD_RTC_DISP_DATE_DATA)
	  {
		 Delay_in_Milli_Sec(DATA_BLINK_MS);	 
		 LCD_String_xy(0, 6, "  "); 
         Delay_in_Milli_Sec(DATA_BLINK_MS);	        
	  }
	  LCD_String_xy(0, 6, var); 		 
	  
  }
  if(display_data == LCD_RTC_DISP_ALL_DATA || display_data == LCD_RTC_DISP_MONTH_DATA)
  {
	  sprintf(var,"/%02u",month);
	  if(display_data == LCD_RTC_DISP_MONTH_DATA)
	  {
		  Delay_in_Milli_Sec(DATA_BLINK_MS);	
          LCD_String_xy(0, 9, "  "); 
          Delay_in_Milli_Sec(DATA_BLINK_MS);
	  }
	  LCD_String_xy(0, 8, var); 	  
  }
  if(display_data == LCD_RTC_DISP_ALL_DATA || display_data == LCD_RTC_DISP_YEAR_DATA)
  {
	  sprintf(var,"/%04lu",year + 2000UL);
	  if(display_data == LCD_RTC_DISP_YEAR_DATA)
      {  
         Delay_in_Milli_Sec(DATA_BLINK_MS);	 
		 LCD_String_xy(0, 12, "    "); 
         Delay_in_Milli_Sec(DATA_BLINK_MS);	         
	  }
	  LCD_String_xy(0, 11, var);
  }
  if(display_data == LCD_RTC_DISP_ALL_DATA || display_data == LCD_RTC_DISP_DAY_DATA)
  {
      LCD_String_xy(0, 16, " ");
	  switch(day)
      {
		  case 1: 
             LCD_String("SUN");
          break;
          case 2:
             LCD_String("MON");
          break;
          case 3:
             LCD_String("TUE");
          break;
          case 4:
             LCD_String("WED");
          break;
          case 5:
            LCD_String("THU");
          break;
          case 6:
            LCD_String("FRI");
          break;
		  case 7:
            LCD_String("SAT");
          break;
      }
  }
  if(display_data == LCD_RTC_DISP_ALL_DATA || display_data == LCD_RTC_DISP_HOUR_DATA)
  {
	  sprintf(var,"%02u",hour);
	  if(display_data == LCD_RTC_DISP_HOUR_DATA)
	  {
		 Delay_in_Milli_Sec(DATA_BLINK_MS);	
		 LCD_String_xy(1, 6, "  "); 
         Delay_in_Milli_Sec(DATA_BLINK_MS);	
	  }
	  LCD_String_xy(1, 6, var); 	  
  }
  if(display_data == LCD_RTC_DISP_ALL_DATA || display_data == LCD_RTC_DISP_MINUTE_DATA)
  {
     sprintf(var,":%02u",minute);
	 if(display_data == LCD_RTC_DISP_MINUTE_DATA)
	 {
		 Delay_in_Milli_Sec(DATA_BLINK_MS);	
		 LCD_String_xy(1, 9, "  "); 
         Delay_in_Milli_Sec(DATA_BLINK_MS);		
	 }
	 LCD_String_xy(1, 8, var); 		     
  }
  if(display_data == LCD_RTC_DISP_ALL_DATA || display_data == LCD_RTC_DISP_SECOND_DATA)
  {
     sprintf(var,":%02u",seconds);
	 if(display_data == LCD_RTC_DISP_SECOND_DATA)
	 {
		 Delay_in_Milli_Sec(DATA_BLINK_MS);	
		 LCD_String_xy(1, 12, "  "); 
         Delay_in_Milli_Sec(DATA_BLINK_MS);			
	 }
	 LCD_String_xy(1, 11, var); 	 
  }  
}     

/*------------------------------------------------------------*-
FUNCTION NAME  : 

DESCRIPTION    :          
								
INPUT          : 

OUTPUT         : 

NOTE           : 
-*------------------------------------------------------------*/
unsigned int BCD_To_Decimal(unsigned int bcdByte)
{
    unsigned int a,b,dec;

    a  = (((bcdByte & 0xF0) >> 4) * 10);
    b = (bcdByte & 0x0F);
    dec = a + b;

    return dec;

}

/*------------------------------------------------------------*-
FUNCTION NAME  : 

DESCRIPTION    :          
								
INPUT          : 

OUTPUT         : 

NOTE           : 
-*------------------------------------------------------------*/
unsigned int Decimal_To_BCD (unsigned int decimalByte)
{

  unsigned int a,b,bcd;

  a=((decimalByte / 10) << 4);
  b= (decimalByte % 10);
  bcd=a|b;

  return bcd;

}
/*------------------------------------------------------------*-
FUNCTION NAME  : 

DESCRIPTION    :          
								
INPUT          : 

OUTPUT         : 

NOTE           : 
-*------------------------------------------------------------*/
unsigned int Day_of_Week(const unsigned date, const unsigned int month, unsigned long int year )
{
    static int t[] = { 0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4 };
	unsigned long int y = year;
	
    y -= month < 3;
    return (y + y / 4 - y / 100 + y / 400 + t[month - 1] + date) % 7;
}
/*------------------------------------------------------------*
FUNCTION NAME  : Str_Fill_Char

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        :    
-*------------------------------------------------------------*/
unsigned int Str_Fill_Char(char *const dest_str, const unsigned char num_chars, const char fill_char)
{
	unsigned char index;
	
	if(dest_str == NULL_PTR || num_chars > STR_MAX_NUM_CHARS)
	{
		UART_Tx_Str("ERR: Str_Fill\r");
		return ERR_INVALID_DATA;
	}
	for (index = 0 ; index < num_chars ; ++index)
	   dest_str[index] = fill_char;
    dest_str[index] = NULL_CHAR;
	
	return SUCCESS;
}
/*------------------------------------------------------------*
FUNCTION NAME  : Delay_in_Milli_Sec

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           :  Function to provide delay Approx 1ms with 11.0592 Mhz crystal

Func ID        :   
-*------------------------------------------------------------*/
void Delay_in_Milli_Sec(unsigned long int count)    		
{
     int i,j;
	 
     for(i=0;i<count;i++)
     for(j=0;j<112;j++);
}

/*------------------------------------------------------------------*-
  ---- END OF FILE -------------------------------------------------
-*------------------------------------------------------------------*/	 
