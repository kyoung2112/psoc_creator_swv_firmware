/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * This file is necessary for your project to build.
 * Please do not delete it.
 *
 * ========================================
*/

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

#define SYNC_COUNT		0x0100		//0x07AF
#define BAUD_DIVISOR	0x0000

#if !(CY_PSOC3)
//struct __FILE { int handle; /* Add whatever you need here */ };
//FILE __stdout;
//FILE __stdin;
#define CYREG_SWV_ITM_SPR_DATA0 CYDEV_ITM_BASE
#define CYREG_SWV_ITM_SPR_DATA1 CYDEV_ITM_BASE+4
#define CYREG_SWV_ITM_SPR_DATA2 CYDEV_ITM_BASE+8
#define CYREG_SWV_ITM_SPR_DATA4 CYDEV_ITM_BASE+16
#endif

void CySwvEnable()
{
	/* Enable serial wire viewer (SWV) on PSoC3 or PSoC5LP */
		
    #if CY_PSOC3
	/* Set up SWV serial output for PSOC3 */
	// Derive the SWV clock from bus clock / 2
	CY_SET_XTND_REG8(CYREG_MLOGIC_DEBUG, (CY_GET_XTND_REG8(CYREG_MLOGIC_DEBUG) | 0x0D));
	
	//Set Output Divisor Register (13 bits)
	CY_SET_REG8(CYDEV_SWV_SWO_CAOSD + 1, (uint8)(BAUD_DIVISOR >> 8));
	CY_SET_REG8(CYDEV_SWV_SWO_CAOSD + 0, (uint8)BAUD_DIVISOR);
	
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
	CY_SET_REG8(CYDEV_TPIU_ASYNC_CLK_PRESCALER + 1, (uint8)(BAUD_DIVISOR >> 8));
	CY_SET_REG8(CYDEV_TPIU_ASYNC_CLK_PRESCALER + 0, (uint8)BAUD_DIVISOR);	

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