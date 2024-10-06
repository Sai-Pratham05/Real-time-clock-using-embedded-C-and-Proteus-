/* ********************************************************************
FILE                 : uart.c

PURPOSE              : 
	 
AUTHOR               : K.M. Arun Kumar alias Arunkumar Murugeswaran
	 
KNOWN BUGS           : 

NOTE                 : 								

CHANGE LOGS          :

*****************************************************************************/

#include "main.h"

// ------  data type declarations ---------------------------

// ------  function prototypes ------------------------------
static void UART_Rx_Char_Proc();

// ------  constants ----------------------------------------


// ------  variables ----------------------------------------
uart_data_t uart_data;


/*------------------------------------------------------------*-
FUNCTION NAME  : UART_Init

DESCRIPTION    : Initialize UART port						 

INPUT          : 

OUTPUT         : 

NOTE           : 
-*------------------------------------------------------------*/
void UART_Init(void)
{
	#ifdef UART_EVENT == INTERRUPT
		ES = 1; //serial Interrupt_Enable
	#endif
	
	SCON = 0x50;    // setup serial port control 
  	TMOD = 0x20;    // hardware (9600 BAUD @11.0592MHZ) 
  	TH1  = 0xFD;    // TH1
  	TR1	 = 1;  		// Timer 1 on
	
	UART_Reset_Datas(UART_RESET_ALL_DATAS);	
	Delay_in_Milli_Sec(1);	
	return;
}

/*------------------------------------------------------------*-
FUNCTION NAME  : UART_Reset_Datas

DESCRIPTION    :					 

INPUT          : 

OUTPUT         : 

NOTE           : 
-*------------------------------------------------------------*/
void UART_Reset_Datas(const unsigned char uart_reset_type)
{
	switch(uart_reset_type)
	{
		case UART_RESET_ALL_DATAS:
		case UART_RESET_ALL_RCVD_DATAS:
		    uart_data.rcvd_ready_flag = STATE_YES;		 		 
		    uart_data.echo_rcvd_data_flag = STATE_YES;
		case UART_RESET_READ_DATA:
    		uart_data.read_flag = STATE_YES;  
		case UART_RESET_RCVD_BUFFER:		
	        Str_Fill_Char(uart_data.rcvd_str, STR_MAX_NUM_CHARS ,NULL_CHAR); 
	        uart_data.rcvd_str_num_chars = 0;
		    uart_data.rcvd_overflow_flag = STATE_NO;
		    uart_data.rcvd_char = NULL_CHAR;
		    uart_data.rcvd_terminator_char_flag = STATE_NO;		   
		if(uart_reset_type != UART_RESET_ALL_DATAS)
			  break;
		case UART_RESET_ALL_TX_DATAS:
          	uart_data.tx_ready_flag = STATE_YES;
		break;	
		
	}
	return;
}
/*------------------------------------------------------------*-
FUNCTION NAME  : UART_Tx_Char

DESCRIPTION    : Transmit Char by polling 				 

INPUT          : 

OUTPUT         : 

NOTE           : 
-*------------------------------------------------------------*/
void UART_Tx_Char(const char tx_char)
{
	if(uart_data.tx_ready_flag == STATE_YES)
	{
		SBUF = tx_char;
		uart_data.tx_ready_flag = STATE_NO; 
        #ifdef UART_EVENT == POLLING
		     while (TI==0); //Transmit Char by polling 
	         TI = 0;
		     uart_data.tx_ready_flag = STATE_YES; 
		#endif
	}		
	return; 
}

/*------------------------------------------------------------*-
FUNCTION NAME  : UART_Tx_Str

DESCRIPTION    : Transmit String by polling 				 

INPUT          : 

OUTPUT         : 

NOTE           : 
-*------------------------------------------------------------*/
void UART_Tx_Str(const char *const tx_str)
{
     unsigned char index;
	 
     for(index = 0; ((*(tx_str + index)) != NULL_CHAR); index++)
		  UART_Tx_Char(*(tx_str + index));      
	 return; 
}

#ifdef UART_EVENT == INTERRUPT
/*------------------------------------------------------------*-
FUNCTION NAME  : Serial_ISR 

DESCRIPTION    : UART by Interrupt 				 

INPUT          : 

OUTPUT         : 

NOTE           : 
-*------------------------------------------------------------*/
void Serial_ISR(void) interrupt 4
{
	if(TI == 1)
	{
	    TI = 0;           //clear uart transmit flag
	    uart_data.tx_ready_flag = STATE_YES; 
	}
	if(RI == 1)
	{
		uart_data.rcvd_ready_flag = STATE_NO; 
		uart_data.rcvd_char = SBUF; 
		UART_Rx_Char_Proc();		
		RI = 0; //clear uart receive flag		
	}
}	

#else
/*------------------------------------------------------------*-
FUNCTION NAME  : UART_Rx_Char

DESCRIPTION    : Receive Char by polling 				 

INPUT          : 

OUTPUT         : 

NOTE           : 
-*------------------------------------------------------------*/
char UART_Rx_Char()
{
	if(uart_data.rcvd_str_num_chars + 1 >= STR_MAX_NUM_CHARS )
	{
		uart_data.rcvd_overflow_flag = STATE_YES;
		return NULL_CHAR;
	}
	uart_data.rcvd_overflow_flag = STATE_NO;
	if(uart_data.rcvd_ready_flag == STATE_YES ) 
	{
		  uart_data.rcvd_ready_flag = STATE_NO; 
	      while (RI==0);
	      uart_data.rcvd_char = SBUF;
		  UART_Rx_Char_Proc();
		  RI = 0;		
		  return(uart_data.rcvd_char);		
	}
	return NULL_CHAR;  	
}
#endif

/*------------------------------------------------------------*-
FUNCTION NAME  :  

DESCRIPTION    : 				 

INPUT          : 

OUTPUT         : 

NOTE           : 
-*------------------------------------------------------------*/
unsigned int UART_Read(const char **const read_str_ptr, const unsigned char *read_num_chars_ptr)
{
	if(read_str_ptr == NULL_PTR || read_num_chars_ptr == NULL_PTR)
	{
		return ERR_NULL_PTR;
	}
	*read_str_ptr = uart_data.rcvd_str;
	read_num_chars_ptr = &uart_data.rcvd_str_num_chars; 
		
	return SUCCESS;
}
/*------------------------------------------------------------*-
FUNCTION NAME  :  UART_Rx_Char_Proc

DESCRIPTION    : 				 

INPUT          : 

OUTPUT         : 

NOTE           : 
-*------------------------------------------------------------*/
static void UART_Rx_Char_Proc()
{
	uart_data.read_flag = STATE_NO;
	if(uart_data.rcvd_str_num_chars + 1 < STR_MAX_NUM_CHARS)
	{
		 uart_data.rcvd_overflow_flag = STATE_NO;
         switch(uart_data.rcvd_char)
		 {
              case BACKSPACE_CHAR:
		         if(uart_data.rcvd_str_num_chars > 0)
			     {
			        if(uart_data.echo_rcvd_data_flag == STATE_YES)
			               UART_Tx_Char(uart_data.rcvd_char);
			         uart_data.rcvd_str[uart_data.rcvd_str_num_chars--] = NULL_CHAR;
			     }
			  break;
              case TERMINATOR_CHAR:
			     if(uart_data.echo_rcvd_data_flag == STATE_YES)
				        UART_Tx_Char(uart_data.rcvd_char);
                 uart_data.rcvd_terminator_char_flag = STATE_YES;
				 uart_data.rcvd_str[uart_data.rcvd_str_num_chars] = NULL_CHAR;
              break;
              default:
			    if(uart_data.echo_rcvd_data_flag == STATE_YES)
			         UART_Tx_Char(uart_data.rcvd_char);
			    uart_data.rcvd_terminator_char_flag = STATE_NO; 
		        uart_data.rcvd_str[uart_data.rcvd_str_num_chars++] = uart_data.rcvd_char;
		 }
	}
	else
        uart_data.rcvd_overflow_flag = STATE_YES;
	uart_data.rcvd_ready_flag = STATE_YES;
	
    return;  		 
}		 
/*-------------------------------------------------------------------
  ------------------------ END OF FILE ------------------------------
-------------------------------------------------------------------*/

