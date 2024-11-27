#include <Common.h>
#include "UART.h"

/*****************************************************************************
 Define
******************************************************************************/

#if defined (__ARMCC_VERSION)
	#if __ARMCC_VERSION < 6000000  
		struct __FILE
		{
			int handle; 
		};
	#endif
#endif

FILE __stdout; //STDOUT

int fputc(int ch, FILE * stream)
{
	//write_ASCII_UART0(ch); //Transmit Character
	write_ASCII_UART (UART_0, ch); // UART0
	return ch; //return the character written to denote a successful write
}
