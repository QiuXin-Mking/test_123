#include "HT1621.h"



/*Command Mode: Command Code = 100  */	

void HT1621_Command (HT1621 *g_HT1621, uint8_t command)
{
	g_HT1621->cmd_done = FALSE;
	g_HT1621->CS = 0xE000000;
	g_HT1621->WR = 0xF555555;
	switch (command)
	{
		case CMD_LCD_ON: /* LCD ON */			
			g_HT1621->DATA = 0xF780007;
			break;
		case CMD_RC_256K: /* RC_256K  */
			g_HT1621->DATA = 0xF01E007;
			break;
		case CMD_BIAS3_COM4: /* BIAS_COM  */
			g_HT1621->DATA = 0xF619807;
			break;
		case CMD_BIAS2_COM4: /* BIAS_COM  */
			g_HT1621->DATA = 0xF609807;
			break;
		case CMD_SYS_EN: /* SYS_EN */
			g_HT1621->DATA = 0xF600007;
			break;
		default:
			break;
	}
	g_HT1621->intr = 28;
	TIMER0->CTL |= TIMER_CTL_TAEN; /* enable timer 1  */
	while (g_HT1621->cmd_done == FALSE){};
}

/* digit - 1,2,3,4,5,6 - starting from right most digit        	*/
/* number - 0 to 9 & blank (10)																	*/
void HT1621_Write_Digit (HT1621 *g_HT1621, uint8_t digit, uint8_t number)
{
	uint32_t addr_1[] = {0,0x000,0x300,0x0C0,0x3C0,0x030,0x330}; // 0,digit1,digit2,digit3,digit4,digit5,digit6
	uint32_t disp_num_1[] = {0xFC,0x3C,0xF0,0xFC,0x3C,0xCC,0xCC,0xFC,0xFC,0xFC,0};// 0,1,2,3,4,5,6,7,8,9,blank
	uint32_t addr_2[] = {0,0xC00,0xF00,0xCC0,0xFC0,0xC30,0xF30}; // 0,digit1,digit2,digit3,digit4,digit5,digit6 
	uint32_t disp_num_2[] = {0xCF,0x00,0x3F,0x33,0xF0,0xF3,0xFF,0x0,0xFF,0xF0,0};// 0,1,2,3,4,5,6,7,8,9,blank
	
	g_HT1621->digit[digit] = number;
	
	g_HT1621->CS = 0xF8000000;
	g_HT1621->WR = 0xFD555555;
	g_HT1621->intr = 31;
	
	// write to 1st addr for digit
	g_HT1621->cmd_done = FALSE;
	g_HT1621->DATA = 0xE0000067 | disp_num_1[number]<<19 | addr_1[digit]<<7;
	TIMER0->CTL |= TIMER_CTL_TAEN; /* enable timer 0  */
	while (g_HT1621->cmd_done == FALSE){};
	
	// write to 2nd addr for digit
	g_HT1621->cmd_done = FALSE;
	g_HT1621->DATA = 0xE0000067 | disp_num_2[number]<<19 | addr_2[digit]<<7;
	TIMER0->CTL |= TIMER_CTL_TAEN; /* enable timer 0  */
	while (g_HT1621->cmd_done == FALSE){};
}

/* dot numbers from 3 to 1; Dot 1 is the right most dot (left side of digit 1    */
/* dot_num=1 to digit 1; dot_num=2 to digit 2; dot_num=3 to digit 3     */
void HT1621_Write_Dot (HT1621 *g_HT1621, uint8_t dot_num, uint8_t dot_on)
{
	uint32_t addr_1[] = {0,0x000,0x300,0x0C0,0x3C0,0x030,0x330}; // 0,digit1,digit2,digit3,digit4,digit5,digit6
	uint32_t disp_num_1[] = {0xFC,0x3C,0xF0,0xFC,0x3C,0xCC,0xCC,0xFC,0xFC,0xFC,0};// 0,1,2,3,4,5,6,7,8,9,blank
	uint32_t addr_2[] = {0,0xC00,0xF00,0xCC0,0xFC0,0xC30,0xF30}; // 0,digit1,digit2,digit3,digit4,digit5,digit6 
	uint32_t disp_num_2[] = {0xCF,0x00,0x3F,0x33,0xF0,0xF3,0xFF,0x0,0xFF,0xF0,0};// 0,1,2,3,4,5,6,7,8,9,blank
	uint32_t dot[] = {0x00,0x03}; // {off, on}

	disp_num_1[g_HT1621->digit[dot_num]] = disp_num_1[g_HT1621->digit[dot_num]] | dot[dot_on]; // update the dot bits
	
	g_HT1621->CS = 0xF8000000;
	g_HT1621->WR = 0xFD555555;
	g_HT1621->intr = 31;
	
	// write to 1st addr for digit 
	g_HT1621->cmd_done = FALSE;
	g_HT1621->DATA = 0xE0000067 | disp_num_1[g_HT1621->digit[dot_num]]<<19 | addr_1[dot_num]<<7;
	TIMER0->CTL |= TIMER_CTL_TAEN; /* enable timer 0  */
	while (g_HT1621->cmd_done == FALSE){};
}
