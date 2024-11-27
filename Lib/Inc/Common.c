#include "Common.h"

void delay_ms( int ms )
{
	volatile int cnt = (SystemCoreClock/1000)*ms;
	
	while( cnt-- )
	{
		__NOP();
	}
}

void delay_us( int us )
{
	volatile int cnt = (SystemCoreClock/1000000)*us;
	
	while( cnt-- )
	{
		__NOP();
	}
}
