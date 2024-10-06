/**************************************************************************
   FILE          :    uart.h
 
   PURPOSE       :    UART Header
 
   AUTHOR        :    K.M. Arun Kumar alias Arunkumar Murugeswaran
 
  KNOWN BUGS     :
	
  NOTE           :  
 
  CHANGE LOGS    :
	   
 **************************************************************************/
 /* to avoid this header file to be included more than once, conditional directive is used  */
#ifndef _UART_H
#define _UART_H
  
typedef enum 
{
	UART_RESET_ALL_RCVD_DATAS, UART_RESET_RCVD_BUFFER, UART_RESET_READ_DATA, UART_RESET_ALL_TX_DATAS, UART_RESET_ALL_DATAS
} uart_reset_t;   
   
typedef struct
{
	char rcvd_str[STR_MAX_NUM_CHARS];
	unsigned char rcvd_str_num_chars; 
	char rcvd_char;  
	unsigned int tx_ready_flag              : 1;
	unsigned int rcvd_ready_flag            : 1;
	unsigned int rcvd_terminator_char_flag  : 1;
	unsigned int read_flag                  : 1; // UART  
	unsigned int rcvd_overflow_flag         : 1; 
    unsigned int echo_rcvd_data_flag        : 1;
    unsigned int                            : 2;   	
} uart_data_t; 

extern uart_data_t uart_data;
  
/* -------------------- public prototype declaration --------------------------------------- */
void UART_Init(void);
void UART_Reset_Datas(const unsigned char uart_reset_type);
void UART_Tx_Char(const char tx_char);
void UART_Tx_Str(const char *const tx_str);
char UART_Rx_Char();
unsigned int UART_Read(const char **const read_str_ptr, const unsigned char *read_num_chars_ptr);

#endif

/*-------------------------------------------------------------------
  ------------------------ END OF FILE ------------------------------
-------------------------------------------------------------------*/
