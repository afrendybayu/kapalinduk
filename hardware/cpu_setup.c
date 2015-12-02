
#include "FreeRTOS.h"
#include "cpu_setup.h"


void sysInit()	{
	#ifdef RUN_FROM_RAM
		/* Remap the interrupt vectors to RAM if we are are running from RAM. */
		SCB_MEMMAP = 2;
	#else
		
	#endif
	
	// Disable the PLL. //
	PLLCON = 0;							// 0 = PLL dimatikan dulu
	PLLFEED = mainPLL_FEED_BYTE1;		// 0xAA
	PLLFEED = mainPLL_FEED_BYTE2;		// 0x55
	
	// Configure clock source. //				// SCS = system control dan status register 
//	pakai internal RC saja
//	SCS |= mainOSC_ENABLE;						// mainOSC_ENABLE = 0x20, Hal 42	==> Untuk Kristal External
//	while( !( SCS & mainOSC_STAT ) );			// 
//	CLKSRCSEL = mainOSC_SELECT; 				// mainOSC_SELECT = 0x01 ==> 
	
	// Setup the PLL to multiply the XTAL input by 4. //
	PLLCFG = ( mainPLL_MUL | mainPLL_DIV );		// set fcc jd 480 MHz
	PLLFEED = mainPLL_FEED_BYTE1;
	PLLFEED = mainPLL_FEED_BYTE2;

	// Turn on and wait for the PLL to lock... //
	PLLCON = mainPLL_ENABLE;					// mainPLL_ENABLE = 0x01
	PLLFEED = mainPLL_FEED_BYTE1;
	PLLFEED = mainPLL_FEED_BYTE2;

	CCLKCFG = mainCPU_CLK_DIV;					// mainCPU_CLK_DIV = 8
	while( !( PLLSTAT & mainPLL_LOCK ) );
	
	// Connecting the clock. //
	PLLCON = mainPLL_CONNECT;					// mainPLL_CONNECT = 0x02 | 0x01
	PLLFEED = mainPLL_FEED_BYTE1;
	PLLFEED = mainPLL_FEED_BYTE2;
	while( !( PLLSTAT & mainPLL_CONNECTED ) ); 
	
	/* 
	This code is commented out as the MAM does not work on the original revision
	LPC2368 chips.  If using Rev B chips then you can increase the speed though
	the use of the MAM.
	
	Setup and turn on the MAM.  Three cycle access is used due to the fast
	PLL used.  It is possible faster overall performance could be obtained by
	tuning the MAM and PLL settings.
	*/
	#if 1
	MAMCR = 0;
	//MAMTIM = mainMAM_TIM_3;			// MAMTIM=1 --> 20MHz, MAMTIM=2 --> 40MHz, MAMTIM=3 >40MHz, MAMTIM=4 >=60MHz
	MAMTIM = mainMAM_TIM_4;				// 
	MAMCR = mainMAM_MODE_FULL;
	#endif
}

void setup_watchdog(void)		{
	WDCLKSEL = 0;				// internal clock 4 MHz
	WDTC = WDOG_TO_RESET;
	WDMOD = WDEN | WDRESET;
	
	// start the watchdog
	pll_feed();
}

void reset_cpu(void)		{
	uprintf("\r\n\r\n... reset cpu ...\r\n");
	
	//WDTC = 125000;				// 500ms
	WDTC = 300;				// 500ms
	WDMOD = WDEN | WDRESET;		// 0x03;
	pll_feed();
}

inline void pll_feed(void)	{
	// harusnya saat ini interupt disable
	// tapi masak tidak ada yang sukses di tendang tiap detik
	WDFEED = mainPLL_FEED_BYTE1;
	WDFEED = mainPLL_FEED_BYTE2;
}
