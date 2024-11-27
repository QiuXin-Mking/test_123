 /*****************************************************************************
 @Project		: MET2304
 @File 			: 
 @Details  	: UART                    
 @Author		: FongFH
 @Hardware	: Tiva LaunchPad TM4C123
 
 --------------------------------------------------------------------------
 @Revision	:
  Ver  	Author    	Date        	Changes
 --------------------------------------------------------------------------
   1.0  FongFH     5 Sep 24  		Initial Release
   
******************************************************************************/

#ifndef UART_DOT_H
#define UART_DOT_H


/*****************************************************************************
 Define
******************************************************************************/
#define PARITY_NONE 	0UL
#define PARITY_ODD 		1UL
#define PARITY_EVEN 	2UL

#define DATABITS5		 	5UL
#define DATABITS6		 	6UL
#define DATABITS7		 	7UL
#define DATABITS8		 	8UL

#define STOP1		 			1UL
#define STOP2		 			2UL

#define UART_0	 			0U
#define UART_1	 			1U
#define UART_2	 			2U
#define UART_3	 			3U
#define UART_4	 			4U
#define UART_5	 			5U
#define UART_6	 			6U
#define UART_7	 			7U

typedef struct _tagUART_Handle
{
	void 	*pUart;
	char	port;
	char	irq;
	int		baud;
	char	databits;
	char	parity;
	char	stop;
}UART_HANDLE, *PUART_HANDLE;

/******************************************************************************
 Global functions
******************************************************************************/


/******************************************************************************
 @Description 	: 
 @param					: 
 @revision			: 1.0.0
******************************************************************************/

void UART_Init(char uart_no, PUART_HANDLE pHandle);

void write_ASCII_UART (char uart_no, char character);

// older functions for testing
void write_ASCII_UART0 (char);
char read_ASCII_UART0 (void);

#endif /* __UART_DOT_H__ */
