/*******************************************************************************
* File Name: SWV.c
* Version 1.0
*
* Description:
*  Provides the source code to the API for the EEPROM component.
*
********************************************************************************
* Copyright 2008-2012, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "SWV.h"

/*******************************************************************************
* Function Name: SWV_Start
********************************************************************************
*
* Summary:
*  Starts the serial wire viewer
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void SWV_Start(void)
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
