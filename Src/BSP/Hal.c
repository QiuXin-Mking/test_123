/*****************************************************************************
 @Project		: MET2304
 @File 			: Hal.c
 @Details  	: All Ports and peripherals configuration                    
 @Author		: fongfh
 @Hardware	: Tiva LaunchPad 
 
 --------------------------------------------------------------------------
 @Revision	:
  Ver  	Author    	Date        	Changes
 --------------------------------------------------------------------------
   1.0  Name     XXXX-XX-XX  		Initial Release
   
******************************************************************************/
#include <Common.h>
#include "Hal.h"


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


/*****************************************************************************
 Implementation
******************************************************************************/

void Port_Init( void )
{
	/* Enable Clock for SSI0 module */
	SYSCTL->RCGCSSI |= SYSCTL_RCGCSSI_R0;
	
	/* Waiting clock ready */
	while( 0 == (SYSCTL->PRSSI & SYSCTL_PRSSI_R0) );
	
	/* enable clock to UART0   */
	SYSCTL->RCGCUART |= SYSCTL_RCGCUART_R0;
	
	/* Wait for UART to be ready */
	while( 0 == (SYSCTL->PRUART & SYSCTL_PRUART_R0) ); 
	
	/*Enable Clocks Gate for Timers */
	SYSCTL->RCGCTIMER	|= SYSCTL_RCGCTIMER_R0  
					| SYSCTL_RCGCTIMER_R1 
					| SYSCTL_RCGCTIMER_R2 
					| SYSCTL_RCGCTIMER_R3 
					| SYSCTL_RCGCTIMER_R4 
					| SYSCTL_RCGCTIMER_R5; 
	
	/* Waiting clock ready */
	while( 0 == (SYSCTL->PRTIMER & SYSCTL_PRTIMER_R0) );
	while( 0 == (SYSCTL->PRTIMER & SYSCTL_PRTIMER_R1) );
	while( 0 == (SYSCTL->PRTIMER & SYSCTL_PRTIMER_R2) );
	while( 0 == (SYSCTL->PRTIMER & SYSCTL_PRTIMER_R3) );
	
	/*Enable Clocks Gate for GPIO */
	SYSCTL->RCGCGPIO |= SYSCTL_RCGCGPIO_R0
					 | SYSCTL_RCGCGPIO_R1
					 | SYSCTL_RCGCGPIO_R2
					 | SYSCTL_RCGCGPIO_R3
					 | SYSCTL_RCGCGPIO_R4
					 | SYSCTL_RCGCGPIO_R5;
	
	/* Waiting clock ready */
	while( 0 == (SYSCTL->PRGPIO & SYSCTL_PRGPIO_R0) );
	while( 0 == (SYSCTL->PRGPIO & SYSCTL_PRGPIO_R1) );
	while( 0 == (SYSCTL->PRGPIO & SYSCTL_PRGPIO_R2) );
	while( 0 == (SYSCTL->PRGPIO & SYSCTL_PRGPIO_R3) );
	while( 0 == (SYSCTL->PRGPIO & SYSCTL_PRGPIO_R4) );
	while( 0 == (SYSCTL->PRGPIO & SYSCTL_PRGPIO_R5) );
	
	/* LED */
	GPIOF->CR |= BIT(PF_LED_RED) | BIT(PF_LED_GREEN) | BIT(PF_LED_BLUE);
	GPIOF->DIR |= BIT(PF_LED_RED) | BIT(PF_LED_GREEN) | BIT(PF_LED_BLUE);
	GPIOF->DEN |= BIT(PF_LED_RED) | BIT(PF_LED_GREEN) | BIT(PF_LED_BLUE);
	GPIOF->AFSEL &= ~(BIT(PF_LED_RED) | BIT(PF_LED_GREEN) | BIT(PF_LED_BLUE));

	/* User button SW1 & SW2 */
	GPIOF->LOCK = GPIO_LOCK_KEY;
	
	GPIOF->CR |= (BIT(PF_SW1) | BIT(PF_SW2));
	GPIOF->DIR &= ~(BIT(PF_SW1) | BIT(PF_SW2));
	GPIOF->DEN |= (BIT(PF_SW1) | BIT(PF_SW2));
	GPIOF->AFSEL &= ~(BIT(PF_SW1) | BIT(PF_SW2));
	GPIOF->PCTL &= ~(BIT(PF_SW1) | BIT(PF_SW2));
	GPIOF->PUR |= (BIT(PF_SW1) | BIT(PF_SW2));
	
	GPIOF->IM &= ~(BIT(PF_SW1) | BIT(PF_SW2));
	GPIOF->IBE &= ~(BIT(PF_SW1) | BIT(PF_SW2)); /* Disable both edge */
	GPIOF->IS &= ~(BIT(PF_SW1) | BIT(PF_SW2)); /* edge detection */
	GPIOF->IEV = ~(BIT(PF_SW1) | BIT(PF_SW2)); /* Falling edge */
	GPIOF->IM |= (BIT(PF_SW1) | BIT(PF_SW2));
	
	/* SSI & LCD */
	GPIOA->LOCK = GPIO_LOCK_KEY;
	
	GPIOA->DIR |= BIT(PA_LCD_SSI0_CS) | BIT(PA_LCD_DC) | BIT(PA_LCD_RESET);
	GPIOA->DEN |= BIT(PA_LCD_SSI0_CS) | BIT(PA_LCD_DC) | BIT(PA_LCD_RESET);
	GPIOA->AFSEL &= ~(BIT(PA_LCD_SSI0_CS) | BIT(PA_LCD_DC) | BIT(PA_LCD_RESET));
	GPIOA->PCTL &= ~( GPIO_PCTL_PA3_M | GPIO_PCTL_PA6_M | GPIO_PCTL_PA7_M );
	
	GPIOA->DEN |= BIT(PA_LCD_SSI0_SCK) | BIT(PA_LCD_SSI0_MOSI);
	GPIOA->AFSEL |= (BIT(PA_LCD_SSI0_SCK) | BIT(PA_LCD_SSI0_MOSI) );
	GPIOA->PCTL &= ~(GPIO_PCTL_PA2_M | GPIO_PCTL_PA5_M);
	GPIOA->PCTL |= (GPIO_PCTL_PA2_SSI0CLK | GPIO_PCTL_PA5_SSI0TX);
	
	GPIOB->DIR |= BIT(PB_LCD_BL);
	GPIOB->DEN |= BIT(PB_LCD_BL);
	
	/* initialize GPIO PA0 (UART0_RX) & PA1 (UART0_TX)   */
	GPIOA->AFSEL |= BIT(PA_UART0_RX) | BIT(PA_UART0_TX);
	GPIOA->DEN |= BIT(PA_UART0_RX) | BIT(PA_UART0_TX);
	GPIOA->AMSEL &= ~( BIT(PA_UART0_RX) | BIT(PA_UART0_TX) );
	GPIOA->PCTL &= ~( GPIO_PCTL_PA0_M | GPIO_PCTL_PA1_M ); /* clear Port C config bits  */
	GPIOA->PCTL |= GPIO_PCTL_PA0_U0RX | GPIO_PCTL_PA1_U0TX;
	
	/* Keypad 3x4 matrix */
	// add your keypad GPIO initialization codes here.
	GPIOD->DIR |= BIT(PD_KEYPAD_ROW0) | BIT(PD_KEYPAD_ROW1) | BIT(PD_KEYPAD_ROW2) | BIT(PD_KEYPAD_ROW3);
	GPIOD->DEN |=  BIT(PD_KEYPAD_ROW0) | BIT(PD_KEYPAD_ROW1) | BIT(PD_KEYPAD_ROW2) | BIT(PD_KEYPAD_ROW3);
	GPIOD->AFSEL &= ~(BIT(PD_KEYPAD_ROW0) | BIT(PD_KEYPAD_ROW1) | BIT(PD_KEYPAD_ROW2) | BIT(PD_KEYPAD_ROW3));
	GPIOD->PCTL &= ~( GPIO_PCTL_PD0_M | GPIO_PCTL_PD1_M | GPIO_PCTL_PD2_M | GPIO_PCTL_PD3_M );

	GPIOE->DIR &= ~(BIT(PE_KEYPAD_COL0) | BIT(PE_KEYPAD_COL1) | BIT(PE_KEYPAD_COL2));
	GPIOE->DEN |= (BIT(PE_KEYPAD_COL0) | BIT(PE_KEYPAD_COL1) | BIT(PE_KEYPAD_COL2));
	GPIOE->AFSEL &= ~(BIT(PE_KEYPAD_COL0) | BIT(PE_KEYPAD_COL1) | BIT(PE_KEYPAD_COL2));
	GPIOE->PCTL &= ~( GPIO_PCTL_PE1_M | GPIO_PCTL_PE2_M | GPIO_PCTL_PE3_M );
	GPIOE->PUR |= (BIT(PE_KEYPAD_COL0) | BIT(PE_KEYPAD_COL1) | BIT(PE_KEYPAD_COL2));
	
	/* initialize buzzer */
	GPIOA->DIR |=BIT(PA_BUZZER);
	GPIOA->DEN |=BIT(PA_BUZZER);
	GPIOA->DATA &=~BIT(PA_BUZZER);
}

















