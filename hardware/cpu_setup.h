
#ifndef __SETUP_CPU__
#define __SETUP_CPU__

#include "FreeRTOS.h"

void sysInit();
void setup_watchdog(void);
void reset_cpu(void);
inline void pll_feed(void);

#define WDOG_TO_RESET	(1000000 * 15)		// 4/4 MHz * 15 detik //
#define WDEN	BIT(0)
#define WDRESET BIT(1)


#define PLL_MUL				60
#define mainPLL_MUL			( ( unsigned portLONG ) ( PLL_MUL - 1 ) )
#define PLL_DIV				1
#define mainPLL_DIV			( ( unsigned portLONG ) (PLL_DIV - 1) )
#define CLK_DIV				8		// 480/8 = 60
//#define CLK_DIV			48		// 480/48 = 10
#define mainCPU_CLK_DIV		( ( unsigned portLONG ) (CLK_DIV-1) )
#define mainPLL_ENABLE		( ( unsigned portLONG ) 0x0001 )
#define mainPLL_CONNECT		( ( ( unsigned portLONG ) 0x0002 ) | mainPLL_ENABLE )
#define mainPLL_FEED_BYTE1	( ( unsigned portLONG ) 0xaa )
#define mainPLL_FEED_BYTE2	( ( unsigned portLONG ) 0x55 )
#define mainPLL_LOCK		( ( unsigned portLONG ) 0x4000000 )
#define mainPLL_CONNECTED	( ( unsigned portLONG ) 0x2000000 )
#define mainOSC_ENABLE		( ( unsigned portLONG ) 0x20 )
#define mainOSC_STAT		( ( unsigned portLONG ) 0x40 )
#define mainOSC_SELECT		( ( unsigned portLONG ) 0x01 )			// Kristal External 
#define intRC_SELECT		( ( unsigned portLONG ) 0x00 )			// Internal RC	==> CANnet jangan pakai ini
#define RTC_SELECT			( ( unsigned portLONG ) 0x10 )			// RTC

/* Constants to setup the MAM. */
#define mainMAM_TIM_3		( ( unsigned portCHAR ) 0x03 )
#define mainMAM_TIM_4		( ( unsigned portCHAR ) 0x04 )
#define mainMAM_MODE_FULL	( ( unsigned portCHAR ) 0x02 )
#endif

