/*****************************************************************************
 @Project		: MET2304
 @File 			: main.c
 @Details  	:
 @Author		: FongFH
 @Hardware	:

 --------------------------------------------------------------------------
 @Revision	:
  Ver  	Author    	Date        	Changes
 --------------------------------------------------------------------------
   1.0       			2 Oct 24  		Initial Release

******************************************************************************/

#include "Common.h"
#include "Hal.h"
#include "BSP.h"
#include "LED.h"
#include "IRQ.h"
#include "spim.h"
#include "LCD_ST7735R.h"
#include "gui.h"
#include "UART.h"

/*****************************************************************************
 Define
******************************************************************************/
#define LCD_BUF_SIZE 4096
#define LCD_UPDATE_MS 10U
#define KEYPAD_UPDATE_MS 50U
#define BUZZER_MS 100U

/*****************************************************************************
 Type definition
******************************************************************************/

/*****************************************************************************
 Global Variables
******************************************************************************/
void GUI_AppDraw(BOOL bFrameStart);

/*****************************************************************************
 Local const Variables
******************************************************************************/
static char KEY_DECODE_TABLE[4][3] =
	{
		{'1', '2', '3'},
		{'4', '5', '6'},
		{'7', '8', '9'},
		{'*', '0', '#'}};

/*****************************************************************************
 Local Variables
******************************************************************************/
static volatile BOOL g_bSystemTick = FALSE;
static volatile BOOL g_bSecTick = FALSE;
static int g_nCount = 0;
int BUZZCOUNT = 0;
int STATE = 0;
static unsigned int g_nTimeSec = 0;
static volatile BOOL g_bToggle = FALSE;

static volatile int g_bSpiDone = FALSE;
static SPIM_HANDLE g_SpimHandle;
static GUI_DATA g_aBuf[LCD_BUF_SIZE];
static GUI_MEMDEV g_MemDev;
static volatile BOOL g_bLCDUpdate = FALSE;
static volatile int g_nLCD = LCD_UPDATE_MS;
static volatile BOOL g_bBacklightOn = TRUE;

static volatile BOOL g_bLcdFree = TRUE;

static volatile BOOL g_nKeypadScan = FALSE;
static volatile int g_nKeypad = KEYPAD_UPDATE_MS;
static unsigned char g_cKey = '-';
BOOL BUZZER = FALSE;
BOOL SECLICK = FALSE;

// UART
static UART_HANDLE g_UartHandle;
volatile char g_UART_CharRx;

/*****************************************************************************
 Local Functions
******************************************************************************/
static void main_LcdInit(void);
static void main_KeyScan(void);
static void main_KeypadOutput(void);

/*****************************************************************************
 Implementation
******************************************************************************/
int main()
{
	Port_Init();
	SystemCoreClockUpdate();
	SysTick_Config(SystemCoreClock / 1000);

	/* SSI initialization */
	NVIC_SetPriority(SSI0_IRQn, 0);

	SpimInit(
		&g_SpimHandle,
		0U,
		25000000U,
		SPI_CLK_INACT_LOW,
		SPI_CLK_RISING_EDGE,
		SPI_DATA_SIZE_8);

	main_LcdInit();
	IRQ_Init();

	for (;;)
	{
		/* LCD update */
		if (FALSE != g_bLCDUpdate)
		{
			if (0 != g_bLcdFree)
			{
				g_bLCDUpdate = FALSE;
				g_bLcdFree = FALSE;

				/* Draw every block. Consumes less time  */
				GUI_Draw_Exe();
			}
		}

		if (FALSE != g_nKeypadScan)
		{
			g_nKeypadScan = FALSE;
			main_KeyScan();
		}
	}
}

/*****************************************************************************
 Callback functions
******************************************************************************/
void SysTick_Handler(void)
{
	g_bSystemTick = TRUE;

	/* Provide system tick */
	g_nCount++;

	if (BUZZER == TRUE)
	{
		BUZZCOUNT++;
		if (BUZZCOUNT % 100 == 0)
		{
			BUZZER_OFF();
			BUZZER = FALSE;
		}
	}

	if (g_nCount == 1000)
	{
		g_bSecTick = TRUE;
		g_nCount = 0;

		/* Keep track of time based on 1 sec interval */
		g_nTimeSec++;
		if (g_nTimeSec > 24 * 60 * 60)
		{
			g_nTimeSec = 0;
		}
		LED_RGB_SET(RGB_RED * (g_nTimeSec & 1U)); /* Toggle LED every 1 sec based on time LSB */
	}

	if (0 != g_nLCD)
	{
		g_nLCD--;
		if (0 == g_nLCD)
		{
			g_nLCD = LCD_UPDATE_MS;
			g_bLCDUpdate = TRUE;
		}
	}

	if (0 != g_nKeypad)
	{
		g_nKeypad--;
		if (0 == g_nKeypad)
		{
			g_nKeypad = KEYPAD_UPDATE_MS;
			g_nKeypadScan = TRUE;
		}
	}
}

void GUI_AppDraw(BOOL bFrameStart)
{
	/* This function invokes from GUI library */
	char buf[128];

	GUI_Clear(ClrBlue); /* Set background to blue.Refer to gui.h */
	GUI_SetFont(&g_FontBookosb16);
	GUI_SetFontBackColor(ClrBlue);
	GUI_PrintString("MET2304", ClrYellow, 25, 8);
	GUI_SetFont(&g_FontBookos16);
	GUI_PrintString("FA24", ClrYellow, 40, 25);
	GUI_SetFont(&g_FontBradhitc16);
	GUI_PrintString("Yuchen Jiang", ClrWhite, 10, 50);

	GUI_SetColor(ClrYellow);
	GUI_DrawFilledRect(0, 80, 127, 139);
	GUI_SetFont(&FONT_Arialbold16);
	GUI_PrintString("Key Pressed", ClrBlack, 15, 90);
	GUI_SetFont(&g_sFontCalibri24);
	sprintf(buf, "%c", g_cKey);
	GUI_PrintString(buf, ClrBlack, 60, 112);

	GUI_SetColor(ClrLightCyan);
	GUI_DrawFilledRect(0, 140, 127, 159);
	// GUI_SetFont( &g_FontDigital720 );
	GUI_SetFont(&g_Font01digit20);
	GUI_SetFontBackColor(ClrLightCyan);
	sprintf(buf, "%02u: %02u: %02u", (g_nTimeSec / 3600) % 24, (g_nTimeSec / 60) % 60, g_nTimeSec % 60);
	GUI_PrintString(buf, ClrBlack, 10, 142);
}

static void main_cbLcdTransferDone(void)
{
	g_bLcdFree = TRUE;
}

static void main_cbGuiFrameEnd(void)
{
	g_bLcdFree = TRUE;
}

/*****************************************************************************
 Local functions
******************************************************************************/
static void main_KeyScan(void)
{
	int Row, Col, input;
	BOOL KeyPressed = FALSE;

	/* Set all rows to high so that if any key is pressed,
	   a falling edge on the column line can be detected */
	KEYPAD_ALL_ROWS_ON();

	for (Row = 0; Row < 4; Row++)
	{
		/* Pull row by row low to determine which button is pressed */
		KEPAD_ROW_MASKED &= ~(1U << Row);

		/* Short delay to stabilize row that has just been pulled low */
		__NOP();
		__NOP();
		__NOP();

		/* Read input */
		input = KEYPAD_COL_IN();

		switch (input)
		{
		case 3: // First Row, 011, first row low. second row, third row high
			Col = 2;
			KeyPressed = TRUE;
			break;
		case 5: // Second Row, 101, second row low. frist row, third row high
			Col = 1;
			KeyPressed = TRUE;
			break;
		case 6: // Third Row, 110, Third row low, first row, second row high
			Col = 0;
			KeyPressed = TRUE;
			break;
		default:
			break;
		}

		if ((KeyPressed == TRUE) && (BUZZER != TRUE) && (SECLICK == FALSE))
		{
			BUZZER_ON();
			BUZZCOUNT = 0;						 // reset count
			BUZZER = TRUE;						 // set buzzer-flag
			SECLICK = TRUE;						 // mark pressed
			g_cKey = KEY_DECODE_TABLE[Row][Col]; // decode key
			break;
		}
	}

	/* If no key is pressed, allow the buzzer to reset for the next key press */
	if (input == 7)
	{
		SECLICK = FALSE;
	}

	/* Reset all rows for next key detection */
	KEYPAD_ALL_ROWS_OFF();
}

static void main_LcdInit(void)
{
	int screenx;
	int screeny;

	/* g_SpimHandle shall be itializaed before use */

	/* Choosing a landscape orientation */
	LcdInit(&g_SpimHandle, LCD_POTRAIT_180);

	/* Get physical LCD size in pixels */
	LCD_GetSize(&screenx, &screeny);

	/* Initialize GUI */
	GUI_Init(
		&g_MemDev,
		screenx,
		screeny,
		g_aBuf,
		sizeof(g_aBuf));

	/* Switch to transfer word for faster performance */
	SpimSetDataSize(&g_SpimHandle, SPI_DATA_SIZE_16);
	GUI_16BitPerPixel(TRUE);

	/* Clear LCD screen to Blue */
	GUI_Clear(ClrBlue);

	/* set font color background */
	GUI_SetFontBackColor(ClrBlue);

	/* Set font */
	GUI_SetFont(&g_sFontCalibri10);

	LCD_AddCallback(main_cbLcdTransferDone);

	GUI_AddCbFrameEnd(main_cbGuiFrameEnd);

	/* Backlight ON */
	LCD_BL_ON();
}

/*****************************************************************************
 Interrupt functions
******************************************************************************/
void GPIOF_Button_IRQHandler(uint32_t Status)
{

	if (0 != (Status & BIT(PF_SW1)))
	{
		GPIOF->ICR = BIT(PF_SW1);
	}

	if (0 != (Status & BIT(PF_SW2)))
	{
		GPIOF->ICR = BIT(PF_SW2);
	}
}