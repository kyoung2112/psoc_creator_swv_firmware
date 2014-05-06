/******************************************************************************
* Project Name		: Firmware SWV Example
* File Name			: main.c
* Version 			: 1.0
* Device Used		: CY8C3866AXI-040, CY8C5858AXI-LP035
* Software Used		: PSoC Creator 3.0
* Compiler    		: ARMGCC, Keil 8051
* Related Hardware	: CY8CKIT-001, CY8CKIT-030, CY8CKIT-050
*
********************************************************************************
* Copyright (2014), Cypress Semiconductor Corporation. All Rights Reserved.
********************************************************************************
* This software is owned by Cypress Semiconductor Corporation (Cypress)
* and is protected by and subject to worldwide patent protection (United
* States and foreign), United States copyright laws and international treaty
* provisions. Cypress hereby grants to licensee a personal, non-exclusive,
* non-transferable license to copy, use, modify, create derivative works of,
* and compile the Cypress Source Code and derivative works for the sole
* purpose of creating custom software in support of licensee product to be
* used only in conjunction with a Cypress integrated circuit as specified in
* the applicable agreement. Any reproduction, modification, translation,
* compilation, or representation of this software except as specified above 
* is prohibited without the express written permission of Cypress.
*
* Disclaimer: CYPRESS MAKES NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, WITH 
* REGARD TO THIS MATERIAL, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
* Cypress reserves the right to make changes without further notice to the 
* materials described herein. Cypress does not assume any liability arising out 
* of the application or use of any product or circuit described herein. Cypress 
* does not authorize its products for use as critical components in life-support 
* systems where a malfunction or failure may reasonably be expected to result in 
* significant injury to the user. The inclusion of Cypress' product in a life-
* support systems application implies that the manufacturer assumes all risk of 
* such use and in doing so indemnifies Cypress against all charges. 
*
* Use of this Software may be limited by and subject to the applicable Cypress
* software license agreement. 
*******************************************************************************/

#include <device.h>
#include <stdio.h>
#include <project.h>

#define CYSWV_ENCODING_MANCHESTER	0x01
#define CYSWV_ENCODING_UART			0x02
#define ENCODING_USE				CYSWV_ENCODING_MANCHESTER

#define TRCENA          0x01000000
#define DTS_ENA			0x02		/* TRACE_CTL: Enable differential timestamps */
#define ITM_ENA			0x01		/* TRACE_CTL: Enable ITM */
#define SYNC_ENA		0x04		/* TRACE_CTL: Enable sync output from SWV */
#define SWV_ENA			0x10		/* TRACE_CTL: Enable SWV behavior */
#define SWV_CLK_ENA			0x04	/* MLOGIC_DEBUG, enable SWV clk  */
#define SWV_CLK_CPU_DIV_2	0x08	/* MLOGIC_DEBUG, set SWV clk to CPU/2 */
#define SWV_CLK			6000u		/* The software requires SWV_CLK = 6000 kHz */

#define SYNC_COUNT		0x0100		//0x07AF
#define BAUD_DIVISOR	0x0000		//Serial output bit rate is SWV src clk / (BAUD_DIVISOR + 1)

#if !(CY_PSOC3)
/* PSoC5LP SWV stimulus register defines */
	#define CYREG_SWV_ITM_SPR_DATA0 CYDEV_ITM_BASE
	#define CYREG_SWV_ITM_SPR_DATA1 CYDEV_ITM_BASE+4
	#define CYREG_SWV_ITM_SPR_DATA2 CYDEV_ITM_BASE+8
	#define CYREG_SWV_ITM_SPR_DATA4 CYDEV_ITM_BASE+16
#endif

void CySwvEnable()
{
	uint8 clkdiv;
	
	/* Enable serial wire viewer (SWV) on PSoC3 or PSoC5LP */
	
	/* Assuming SWV_CLK is CPU CLK (bus clk) / 2. Need to configure divider to output the right freq 
	Include a warning if the divider does not produce the exact frequency required */
	#if (BCLK__BUS_CLK__KHZ % (2u*SWV_CLK)) == 0
	clkdiv = (BCLK__BUS_CLK__KHZ / (2u*SWV_CLK))-1;	
	#else
	/* BUS CLK / 2 must be able to be divided down with an integer value to reach SWV_CLK 
	To fix, either change your BUS CLK or change your SWV divider (might require a change
	to your PC application to change the Miniprog3 clock frequency) */
	#warning Accurate SWV divider could not be reached.
	#endif
	clkdiv = (BCLK__BUS_CLK__KHZ / (2u*SWV_CLK))-1;	
	
    #if CY_PSOC3
	/* Set up SWV serial output for PSOC3 */
	// Derive the SWV clock from bus clock / 2
	CY_SET_XTND_REG8(CYREG_MLOGIC_DEBUG, (CY_GET_XTND_REG8(CYREG_MLOGIC_DEBUG) | 0x0D));
	
	//Set Output Divisor Register (13 bits)
	CY_SET_REG8(CYDEV_SWV_SWO_CAOSD + 1, (uint8)(clkdiv >> 8));
	CY_SET_REG8(CYDEV_SWV_SWO_CAOSD + 0, (uint8)(clkdiv));
	
	//Setup the output encoding style
	CY_SET_REG8(CYDEV_SWV_SWO_SPP, ENCODING_USE);
	
	//Set trace enable registers
	CY_SET_REG8(CYDEV_SWV_ITM_TER + 3, 0x00);
	CY_SET_REG8(CYDEV_SWV_ITM_TER + 2, 0x00);
	CY_SET_REG8(CYDEV_SWV_ITM_TER + 1, 0x00);
	CY_SET_REG8(CYDEV_SWV_ITM_TER + 0, 0x07); //enable 3 stimulus registers: CYREG_SWV_ITM_SPR_DATA0..2
	
	//Set sync counter
	CY_SET_REG8(CYREG_SWV_ITM_SCR + 1, (uint8)(SYNC_COUNT >> 8));
	CY_SET_REG8(CYREG_SWV_ITM_SCR + 0, (uint8)SYNC_COUNT);
	
	//Enable output features
	CY_SET_REG8(CYDEV_SWV_ITM_CR, ITM_ENA);
	#else //PSOC5
	/* Set up SWV serial output on P1[3] for PSoC5 */
		
	//Enable output features
	CY_SET_REG32(CYDEV_CORE_DBG_EXC_MON_CTL,TRCENA);
	
	// Set following bits in the MLOGIC_DEBUG register:
	// swv_clk_en = 1, swv_clk_sel = 1 (CPUCLK/2).
	CY_SET_REG8(CYREG_MLOGIC_DEBUG, (CY_GET_REG8(CYREG_MLOGIC_DEBUG) | SWV_CLK_ENA | SWV_CLK_CPU_DIV_2));
	
	//enable ITM CR write privs
	CY_SET_REG32(CYDEV_ITM_LOCK_ACCESS,0xC5ACCE55);	//enable ITM CR privs
	
	//Enable ITM
	CY_SET_REG32(CYDEV_ITM_TRACE_CTRL, (uint32) (SWV_ENA | ITM_ENA | SYNC_ENA));

	//Set Output Divisor Register (13 bits)
	CY_SET_REG32(CYDEV_TPIU_ASYNC_CLK_PRESCALER,(uint32)(clkdiv));

	//Setup the output encoding style
	CY_SET_REG8(CYDEV_TPIU_PROTOCOL, ENCODING_USE);
	
	//Set trace enable registers
	CY_SET_REG32(CYDEV_ITM_TRACE_EN, 0x00000007); //enable 3 stimulus register
	
	//Set sync counter
	CY_SET_REG8(CYDEV_ETM_SYNC_FREQ + 1, (uint8)(SYNC_COUNT >> 8));
	CY_SET_REG8(CYDEV_ETM_SYNC_FREQ + 0, (uint8)SYNC_COUNT);
	
	CY_SET_REG32(CYDEV_TPIU_FORM_FLUSH_CTRL,0x00000000);   // Formatter and Flush Control Register

	#endif
	
}

int main(void)
{
	int i;
	CySwvEnable();
	
	while (1)
	{
		printf("test count %u\n",i++);
		CyDelay(5);	/* delay 5ms */
	}
}
#if CY_PSOC3
/* PSoC3: Redefine putchar to direct printf to SWV register 0 (single byte) */
char putchar (char c)
{
	while(CY_GET_REG8(CYREG_SWV_ITM_SPR_DATA0) == 0);
	CY_SET_REG8(CYREG_SWV_ITM_SPR_DATA0,c);
	return c;
}
#else
/* GCC for PSoC5: Redefine _write to direct printf to SWV register 0 (single byte) */
int _write(int file, char *ptr, int len) 
{
    int i;

    for (i = 0; i < len; i++) 
    {
		while (CY_GET_REG8(CYDEV_ITM_BASE) == 0);
    	CY_SET_REG8(CYDEV_ITM_BASE,*ptr++);
    }
    return len;
}

#endif