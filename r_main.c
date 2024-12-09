/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products.
* No other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
* applicable laws, including copyright laws. 
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING THIS SOFTWARE, WHETHER EXPRESS, IMPLIED
* OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NON-INFRINGEMENT.  ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY
* LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE FOR ANY DIRECT,
* INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR
* ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability 
* of this software. By using this software, you agree to the additional terms and conditions found by accessing the 
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2011, 2023 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/

/***********************************************************************************************************************
* File Name    : r_main.c
* Version      : CodeGenerator for RL78/G13 V2.05.07.02 [17 Nov 2023]
* Device(s)    : R5F100MG
* Tool-Chain   : CCRL
* Description  : This file implements main function.
* Creation Date: 12/6/2024
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"
#include "r_cg_cgc.h"
#include "r_cg_port.h"
#include "r_cg_serial.h"
#include "r_cg_timer.h"
/* Start user code for include. Do not edit comment generated here */
#include "fpga.h"
#include "eeprom.h"
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Pragma directive
***********************************************************************************************************************/
/* Start user code for pragma. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */
#define FPGA_PROG_B		P5_bit.no0
#define FPGA_INIT_B		P0_bit.no1
#define FPGA_DONE		P1_bit.no0	//(P1 & 0x01)
#define FPGA_DIN		P1_bit.no2
#define FPGA_CCLK	P1_bit.no0

#define PROG_B_H	P5 |= 0x01
#define PROG_B_L	P5 &= 0xfe

#define DONE_H		P1 |= 0x01
#define DONE_L		P1 &= 0xfe

#define DIN_H		P1 |= 0x04
#define DIN_L		P1 &= 0xfb

#define CCLK_H		P1 |= 0x01
#define CCLK_L		P1 &= 0xfe

#define SW		P14 & 0x03

#define mySizeof(var) ((char*)(&var + 1) - (char*)(&var))



#define RX_BUFFER_SIZE 7  // Size of the expected message: <STX>01a<ETX>
char rx_buffer[RX_BUFFER_SIZE];  // Buffer to hold the received message


volatile uint8_t RX_Data = 0;
volatile uint8_t RX_Buffer[7] = {0};// 20
volatile uint8_t RX_Index = 0;



void fpga_config(void);

/* End user code. Do not edit comment generated here */
void R_MAIN_UserInit(void);

/***********************************************************************************************************************
* Function Name: main
* Description  : This function implements main function.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void main(void)
{
    R_MAIN_UserInit();
    /* Start user code. Do not edit comment generated here */
   
    
      
    while (1U)
    {
	    fpga_config();
    }
    /* End user code. Do not edit comment generated here */
}

/***********************************************************************************************************************
* Function Name: R_MAIN_UserInit
* Description  : This function adds user code before implementing main function.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_MAIN_UserInit(void)
{
    /* Start user code. Do not edit comment generated here */



    EI();	
    /* End user code. Do not edit comment generated here */
}

/* Start user code for adding. Do not edit comment generated here */
extern volatile uint16_t  g_csi00_tx_count;            /* csi00 send data count */
extern unsigned int g_Sendend;

extern void wait_05us( unsigned short cnt );

void delay(volatile uint32_t count) {
    while (count--) {
    }
}
#include <stdlib.h>                       // For malloc and free
#include <string.h>



//**************************************************
//***************Button Handling********************
//**************************************************
#include "r_cg_button_handling.h"

#pragma interrupt r_tau0_channel0_interrupt(vect=INTTM00)
static void __near r_tau0_channel0_interrupt(void) {
    processButtons();
}

//**************************************************
//******************LCD 16x2************************
//**************************************************
#include "r_cg_lcd.h"

//button test
void ledControl(){
    // Check if P14.1 (button) is pressed (adjusted for pull-up logic)
    if ((P12 & (1 << 0))) { 
/*    
        P6 |= (1 << 4);  // Set P6.4 high (LED on)
        P6 |= (1 << 5);  // Set P6.5 high (LED on)
        P6 |= (1 << 6);  // Set P6.6 high (LED on)
        P6 |= (1 << 7);  // Set P6.7 high (LED on)
*/        
//        delay(100000);  // Delay for some time    
     
        P6 &= ~(1 << 4);  // Set P6.4 low (LED off)
        P6 &= ~(1 << 5);  // Set P6.5 low (LED off)
        P6 &= ~(1 << 6);  // Set P6.6 low (LED off)
        P6 &= ~(1 << 7);  // Set P6.7 low (LED off)
        
        delay(100000);  // Delay for some time

       
    } else {
        // Button not pressed
	P6 |= (1 << 4);  // Set P6.4 high (LED on)
        P6 |= (1 << 5);  // Set P6.5 high (LED on)
        P6 |= (1 << 6);  // Set P6.6 high (LED on)
        P6 |= (1 << 7);  // Set P6.7 high (LED on)
    }
}

//uint8_t versionDisplayed = 0;

void fpga_config(void)
{   	//init_Uart();

	//Timer interupt init
	R_TAU0_Channel0_Start();
	
	
	initializeButtons(); // Initialize buttons
		
while (1) {
	processButtons();
	
	//ledControl();
	
	};	   
}

#define FPGA_DATA_CNT 256		//FPGA転送バイト数
#define FPGA_DATA_OFF 0x67		//データオフセット

unsigned short fpga_config2(void)
{
	unsigned long lp1;
	unsigned long tmp = 0;
	unsigned long tmp1 = 0;
	unsigned char data[FPGA_DATA_CNT];
	
	FPGA_PROG_B = 1;			//PROG_B = H

	while( FPGA_INIT_B == 0 )	//INIT_BがHになるのを待つ
		;

	wait_1ms(1);				//1ms待ち

	//FPGAデータ読み出し、データ送信数を取得する
	eeprom_read( 0, FPGA_DATA_CNT, &data[0] );	//FPGAデータ読み出し
	tmp  = data[0x64 + 0] * 0x10000;	//送信データバイト数の取得
	tmp += data[0x64 + 1] * 0x100;
	tmp += data[0x64 + 2];
	
	if( tmp == 0xfffff ){	//EEPROM未書き込み？
		return 0;
	}
	
	FPGA_PROG_B = 0;			//PROG_B = L　FPGAコンフィグレーションクリア
	wait_05us(2);				//1us待ち（PROG_BのLパルス500ns以上）
	
	while( FPGA_INIT_B != 0 )	//INIT_BがLになるのを待つ
		;

	FPGA_PROG_B = 1;			//PROG_B = H　FPGA初期化開始
	
	while( FPGA_INIT_B == 0 )	//INIT_BがHになるのを待つ（FPGA初期化完了待ち）
		;

	wait_1ms(1);				//1ms待ち（500ns以上待つ）
//	for( lp = 0 ; lp < 5000; lp++){	//1ms待ち
//		NOP();
//	}

//	tmp += FPGA_DATA_OFF; 				//データではないヘッダの容量を加算
	
	lp1 = 0;
	g_Sendend = 0;

	while( tmp > 0 ){
		eeprom_read( lp1 * FPGA_DATA_CNT + FPGA_DATA_OFF, FPGA_DATA_CNT, &data[0] );	//FPGAデータ読み出し

		if( tmp > FPGA_DATA_CNT ){
			tmp1 = FPGA_DATA_CNT;
		}
		else{
			tmp1 = tmp;
		}

		g_Sendend = 1;
		//R_CSI00_Start();
		//R_CSI00_Send( &data[0], tmp1 );	//FPGAデータ送信開始
		
		while(g_Sendend != 0) 	//送信完了待ち
			;

		tmp = tmp - tmp1;
		lp1 = lp1 + 1;
		
	}

	while( FPGA_DONE != 0 )	//FPGAプログラム完了待ち   
		;

	return 1;
}


/* End user code. Do not edit comment generated here */
