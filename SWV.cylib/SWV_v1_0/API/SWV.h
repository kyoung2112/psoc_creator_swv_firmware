/*******************************************************************************
* File Name: `$INSTANCE_NAME`.h
* Version `$CY_MAJOR_VERSION`.`$CY_MINOR_VERSION`
*
* Description:
*  Provides the function definitions for the SWV APIs.
*
********************************************************************************
* Copyright 2008-2012, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#include <stdio.h>                  // For printf
#include <cytypes.h>                // For GETREG etc.

// ============================================================================
// Definitions
// ============================================================================

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

#if !(CY_PSOC3)
    /* PSoC5LP SWV stimulus register defines */
	#define CYREG_SWV_ITM_SPR_DATA0 CYDEV_ITM_BASE
	#define CYREG_SWV_ITM_SPR_DATA1 CYDEV_ITM_BASE+4
	#define CYREG_SWV_ITM_SPR_DATA2 CYDEV_ITM_BASE+8
	#define CYREG_SWV_ITM_SPR_DATA4 CYDEV_ITM_BASE+16
#endif

// ============================================================================
// API Prototypes
// ============================================================================
void `$INSTANCE_NAME`_Start(void);

