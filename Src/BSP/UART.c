/*****************************************************************************
 @Project		: MET2304
 @File 			: UART.c
 @Details  	:                     
 @Author		: FongFH 
 @Hardware	: Tiva LaunchPad TM4C123
 
 --------------------------------------------------------------------------
 @Revision	:
  Ver  	Author    	Date        	Changes
 --------------------------------------------------------------------------
   1.0  FongFH  		5 Sep 24		Initial Release
   
******************************************************************************/
#include <Common.h>
#include "UART.h"

/*****************************************************************************
 Define
******************************************************************************/


/*****************************************************************************
 Type definition
******************************************************************************/


/*****************************************************************************
 Global Variables
******************************************************************************/


/*****************************************************************************
 Local Variables
******************************************************************************/
static PUART_HANDLE	g_pUartHandle;

/*****************************************************************************
 Implementation
******************************************************************************/
void UART_Init(char uart_no, PUART_HANDLE pHandle)
{
	g_pUartHandle = pHandle;
	UART0_Type *uart;
	float tmp;
	
	g_pUartHandle->port = uart_no;
	
	switch (g_pUartHandle->port)
	{
		case 0:
			 uart = UART0;
		break;
		case 1:
			 uart = UART1;
		break;
		case 2:
			 uart = UART2;
		break;
		case 3:
			 uart = UART3;
		break;
		case 4:
			 uart = UART4;
		break;
		case 5:
			 uart = UART5;
		break;
		case 6:
			 uart = UART6;
		break;
		case 7:
			 uart = UART7;
		break;
		default: break;
	}
	
	/** initialize UART0   **/	
	uart->CTL &= ~UART_CTL_UARTEN; 					/* disable UART during initialization  	*/
	uart->CC &= ~UART_CC_CS_M; 							//  clock source mask
	uart->CC |= UART_CC_CS_SYSCLK; 					// set to system clock
	
	uart->CTL &= ~UART_CTL_HSE;				/* use 16X CLKDIV  							*/
	
	uart->LCRH &= ~UART_LCRH_WLEN_M; 	
	
	// word length
	switch (g_pUartHandle->databits)
	{
		case DATABITS5:
			uart->LCRH |= UART_LCRH_WLEN_5;
		break;
		case DATABITS6:
			uart->LCRH |= UART_LCRH_WLEN_6;
		break;
		case DATABITS7:
			uart->LCRH |= UART_LCRH_WLEN_7;
		break;
		case DATABITS8:
			uart->LCRH |= UART_LCRH_WLEN_8;
		break;
		default: break;
	}
	
	// parity
	switch (g_pUartHandle->parity)
	{
		case PARITY_NONE:
			uart->LCRH &= ~UART_LCRH_PEN;// disable parity
		break;
		case PARITY_ODD:
			uart->LCRH |= UART_LCRH_PEN;
			uart->LCRH &= ~UART_LCRH_EPS ;   /* odd parity */ 	
		break;
		case PARITY_EVEN:
			uart->LCRH |= UART_LCRH_PEN;
			uart->LCRH |= UART_LCRH_EPS ;   /* even parity */ 	
		break;
		default: break;
	}
	
	/* Stop bits							*/
	switch (g_pUartHandle->stop)
	{
		case STOP1:
			uart->LCRH &= ~UART_LCRH_STP2; 	/* 1 stop bit `*/
		break;
		case STOP2:
			uart->LCRH |= UART_LCRH_STP2; 	/* 2 stop bit `*/
		break;
		default: break;
	}
	
	// Baud rate calculations: 80 MHz
	//	uart->IBRD = 5;	 /* int (80,000,000 / (16 * 921,600)) = 5.425347    */
	//	uart->FBRD = 27; /* int (5.425347 * 64 + 0.5 = int (27.72) = 27   */
	tmp = (SystemCoreClock*1.0)/(16 * g_pUartHandle->baud);
	uart->IBRD = (int)tmp;
	uart->FBRD = (int)((tmp * 64) + 0.5);
	
	uart->LCRH |= UART_LCRH_FEN; 										/* enable FIFO*/
	
	//UART0->CTL |= UART_CTL_TXE; 			/* transmit enable 							*/
	uart->CTL |= UART_CTL_TXE | UART_CTL_RXE ; 			/* transmit & receive enable 							*/
	uart->CTL |= UART_CTL_UARTEN; 										/* enable UART0   		*/
}

/** Write an ASCII character to UART0				**/
/** character = ASCII to write 							**/
void write_ASCII_UART0 (char character )
{
	while( 0 != (UART0->FR & UART_FR_TXFF) ){}; 	/* wait if TX FIFO full					*/
	UART0->DR = character; 												/* write character to UART data reg */
}

void write_ASCII_UART (char uart_no, char character)
{
	UART0_Type *uart;
	
	switch (uart_no)
	{
		case 0:
			 uart = UART0;
		break;
		case 1:
			 uart = UART1;
		break;
		case 2:
			 uart = UART2;
		break;
		case 3:
			 uart = UART3;
		break;
		case 4:
			 uart = UART4;
		break;
		case 5:
			 uart = UART5;
		break;
		case 6:
			 uart = UART6;
		break;
		case 7:
			 uart = UART7;
		break;
		default: break;
	}
	while( 0 != (uart->FR & UART_FR_TXFF) ){}; 	/* wait if TX FIFO full					*/
	uart->DR = character; 												/* write character to UART data reg */
}

// receive character from UART0
char read_ASCII_UART0 (void)
{
	while (0 != (UART0->FR & UART_FR_RXFE)){} /* loop if Rx FIFO is empty */
	return (UART0->DR);
}
