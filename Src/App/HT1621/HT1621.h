/*****************************************************************************
 @Project		: MET2304
 @File 			: 
 @Details  	: HT1621                    
 @Author		: FongFH
 @Hardware	: Tiva LaunchPad TM4C123
 
 --------------------------------------------------------------------------
 @Revision	:
  Ver  	Author    	Date        	Changes
 --------------------------------------------------------------------------
   1.0  FongFH     5 Sep 24  		Initial Release
   
******************************************************************************/

#ifndef HT1621_DOT_H
#define HT1621_DOT_H

#include "Common.h"

typedef struct tag_HT1621 {
	uint64_t CS;
	uint64_t WR;
	uint64_t DATA;
	uint8_t intr; 			// no of intr
	uint8_t cmd_done; 
	uint8_t digit[7];   // stores current values of the digits
} HT1621; 

#define CMD_LCD_ON 				1
#define CMD_RC_256K				2
#define CMD_BIAS2_COM4 		3
#define CMD_BIAS3_COM4 		4
#define CMD_SYS_EN 				5


/* write a command to HT1621                            */
/*    - g_HT1621 = pointer to HT1621 data structure     */
/*    - command = command number (1 -5, see above)      */
void HT1621_Command (HT1621 *g_HT1621, uint8_t command);

/* writes a 7-segment digit to HT1621 LCD                       */         
/* digit - 1,2,3,4,5,6 - starting from right most digit        	*/
/* number - 0 to 9 & blank (10)																	*/
void HT1621_Write_Digit (HT1621 *g_HT1621, uint8_t digit, uint8_t number);

/* writes a dot to HT1621 LCD                                                    */     
/* dot numbers from 3 to 1; Dot 1 is the right most dot (left side of digit 1    */
/* dot_num=1 to digit 1; dot_num=2 to digit 2; dot_num=3 to digit 3              */
void HT1621_Write_Dot (HT1621 *g_HT1621, uint8_t dot_num, uint8_t dot_on);

#endif