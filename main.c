/*
    FreeRTOS V7.5.2 - Copyright (C) 2013 Real Time Engineers Ltd.

    VISIT http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS provides completely free yet professionally developed,    *
     *    robust, strictly quality controlled, supported, and cross          *
     *    platform software that has become a de facto standard.             *
     *                                                                       *
     *    Help yourself get started quickly and support the FreeRTOS         *
     *    project by purchasing a FreeRTOS tutorial book, reference          *
     *    manual, or both from: http://www.FreeRTOS.org/Documentation        *
     *                                                                       *
     *    Thank you!                                                         *
     *                                                                       *
    ***************************************************************************

    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation >>!AND MODIFIED BY!<< the FreeRTOS exception.

    >>! NOTE: The modification to the GPL is included to allow you to distribute
    >>! a combined work that includes FreeRTOS without being obliged to provide
    >>! the source code for proprietary components outside of the FreeRTOS
    >>! kernel.

    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  Full license text is available from the following
    link: http://www.freertos.org/a00114.html

    1 tab == 4 spaces!

    ***************************************************************************
     *                                                                       *
     *    Having a problem?  Start by reading the FAQ "My application does   *
     *    not run, what could be wrong?"                                     *
     *                                                                       *
     *    http://www.FreeRTOS.org/FAQHelp.html                               *
     *                                                                       *
    ***************************************************************************

    http://www.FreeRTOS.org - Documentation, books, training, latest versions,
    license and Real Time Engineers Ltd. contact details.

    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool, a DOS
    compatible FAT file system, and our tiny thread aware UDP/IP stack.

    http://www.OpenRTOS.com - Real Time Engineers ltd license FreeRTOS to High
    Integrity Systems to sell under the OpenRTOS brand.  Low cost OpenRTOS
    licenses offer ticketed support, indemnification and middleware.

    http://www.SafeRTOS.com - High Integrity Systems also provide a safety
    engineered and independently SIL3 certified version for use in safety and
    mission critical applications that require provable dependability.

    1 tab == 4 spaces!
*/

/*
 * Creates all the demo application tasks, then starts the scheduler.  The WEB
 * documentation provides more details of the standard demo application tasks.
 * In addition to the standard demo tasks, the following tasks and tests are
 * defined and/or created within this file:
 *
 * "LCD" task - the LCD task is a 'gatekeeper' task.  It is the only task that
 * is permitted to access the display directly.  Other tasks wishing to write a
 * message to the LCD send the message on a queue to the LCD task instead of
 * accessing the LCD themselves.  The LCD task just blocks on the queue waiting
 * for messages - waking and displaying the messages as they arrive.
 *
 * "Check" hook -  This only executes every five seconds from the tick hook.
 * Its main function is to check that all the standard demo tasks are still 
 * operational.  Should any unexpected behaviour within a demo task be discovered 
 * the tick hook will write an error to the LCD (via the LCD task).  If all the 
 * demo tasks are executing with their expected behaviour then the check task 
 * writes PASS to the LCD (again via the LCD task), as described above.
 *
 * "uIP" task -  This is the task that handles the uIP stack.  All TCP/IP
 * processing is performed in this task.
 */

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* Demo app includes. */
#include "monita.h"
#include "hardware.h"
#include "ff/ff9b/src/ff.h"

/* Demo application definitions. */
#define mainQUEUE_SIZE						( 3 )
#define mainCHECK_DELAY						( ( portTickType ) 5000 / portTICK_RATE_MS )
#define mainBASIC_WEB_STACK_SIZE            ( configMINIMAL_STACK_SIZE * 6 )

/* Task priorities. */
#define mainQUEUE_POLL_PRIORITY				( tskIDLE_PRIORITY + 2 )
#define mainCHECK_TASK_PRIORITY				( tskIDLE_PRIORITY + 3 )
#define mainBLOCK_Q_PRIORITY				( tskIDLE_PRIORITY + 2 )
#define mainFLASH_PRIORITY                  ( tskIDLE_PRIORITY + 2 )
#define mainCREATOR_TASK_PRIORITY           ( tskIDLE_PRIORITY + 3 )
#define mainGEN_QUEUE_TASK_PRIORITY			( tskIDLE_PRIORITY ) 

/* Constants to setup the PLL. */
#if 0
#define mainPLL_MUL			( ( unsigned portLONG ) ( 8 - 1 ) )
#define mainPLL_DIV			( ( unsigned portLONG ) 0x0000 )
#define mainCPU_CLK_DIV		( ( unsigned portLONG ) 0x0003 )
#define mainPLL_ENABLE		( ( unsigned portLONG ) 0x0001 )
#define mainPLL_CONNECT		( ( ( unsigned portLONG ) 0x0002 ) | mainPLL_ENABLE )
#define mainPLL_FEED_BYTE1	( ( unsigned portLONG ) 0xaa )
#define mainPLL_FEED_BYTE2	( ( unsigned portLONG ) 0x55 )
#define mainPLL_LOCK		( ( unsigned portLONG ) 0x4000000 )
#define mainPLL_CONNECTED	( ( unsigned portLONG ) 0x2000000 )
#define mainOSC_ENABLE		( ( unsigned portLONG ) 0x20 )
#define mainOSC_STAT		( ( unsigned portLONG ) 0x40 )
#define mainOSC_SELECT		( ( unsigned portLONG ) 0x01 )
#endif

/* Constants to setup the MAM. */
#define mainMAM_TIM_3		( ( unsigned portCHAR ) 0x03 )
#define mainMAM_MODE_FULL	( ( unsigned portCHAR ) 0x02 )


static void vLedTask( void *pvParameters );
/* The queue used to send messages to the LCD task. */
//xQueueHandle xLCDQueue;
xTaskHandle *hdl_led;

#ifdef PAKAI_SHELL
xSemaphoreHandle xSemSer0 = NULL;
#endif

/*-----------------------------------------------------------*/

int main( void )	{
	st_hw.init = 0;
	st_hw.idle_c = 0;
	st_hw.mm = 0;
	st_hw.uudetik = 0;
	st_hw.uuwaktu = 0;

	#ifdef PAKAI_SHELL
		xSemSer0 = xSemaphoreCreateMutex();
	#endif

	setup_hardware();
	init_hardware();
	
	#if 0
	struct t_env *st_env;
	st_env = pvPortMalloc( sizeof (struct t_env) );
	if (st_env == NULL)	{
		printf(" %s(): ERR allok memory gagal !\r\n", __FUNCTION__);
		vPortFree(st_env);
	}

	memcpy((char *) st_env, (char *) ALMT_ENV, (sizeof (struct t_env)));	
	jml_mavg = hitung_mavg();
	st_env->n_mavg = jml_mavg;
	simpan_st_rom(SEKTOR_ENV, ENV, 1, (unsigned short *) st_env, 0);
	vPortFree (st_env);
	#endif
	
	#if 0
	st_mavg = pvPortMalloc( jml_mavg * sizeof (struct t_mavg) );
	if (st_mavg == NULL)	{
		printf(" %s(): ERR allok memory gagal !\r\n", __FUNCTION__);
		vPortFree (st_mavg);
	}
	ngurut_mavg();
	#endif
	
	xTaskCreate( vLedTask, ( signed portCHAR * ) "Led", configMINIMAL_STACK_SIZE*ST_LED, NULL, \
		tskIDLE_PRIORITY, ( xTaskHandle * ) &hdl_led );
	
	FIO1CLR = LED_UTAMA;
	init_ambilCepatTasks();

	/* Start the scheduler. */
	vTaskStartScheduler();

    /* Will only get here if there was insufficient memory to create the idle
    task. */
	return 0; 
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook()	{
	st_hw.idle_c++;
}

void vApplicationTickHook( void )
{
#if 0
unsigned portBASE_TYPE uxColumn = 0;
static xLCDMessage xMessage = { 0, "PASS" };
static unsigned portLONG ulTicksSinceLastDisplay = 0;
static portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

	/* Called from every tick interrupt.  Have enough ticks passed to make it
	time to perform our health status check again? */
	ulTicksSinceLastDisplay++;
	if( ulTicksSinceLastDisplay >= mainCHECK_DELAY )
	{
		ulTicksSinceLastDisplay = 0;
		
		/* Has an error been found in any task? */

        if( xAreBlockingQueuesStillRunning() != pdTRUE )
		{
			xMessage.pcMessage = "ERROR - BLOCKQ";
		}

		if( xAreBlockTimeTestTasksStillRunning() != pdTRUE )
		{
			xMessage.pcMessage = "ERROR - BLOCKTIM";
		}

		if( xAreGenericQueueTasksStillRunning() != pdTRUE )
		{
			xMessage.pcMessage = "ERROR - GENQ";
		}
		
		if( xAreQueuePeekTasksStillRunning() != pdTRUE )
		{
			xMessage.pcMessage = "ERROR - PEEKQ";
		}       
		
		if( xAreDynamicPriorityTasksStillRunning() != pdTRUE )
		{
			xMessage.pcMessage = "ERROR - DYNAMIC";
		}
        
        xMessage.xColumn++;

		/* Send the message to the LCD gatekeeper for display. */
		xHigherPriorityTaskWoken = pdFALSE;
		xQueueSendToBackFromISR( xLCDQueue, &xMessage, &xHigherPriorityTaskWoken );
	}
#endif
}
/*-----------------------------------------------------------*/

inline void stat_ps()	{
	st_hw.idle   = st_hw.idle_c;
	st_hw.idle_c = 0;
	st_hw.adc_pd = st_hw.adc_c;
	st_hw.adc_c	 = 0;
}

inline void init_led()		{
	vTaskDelay(400);
	FIO1CLR = LED_UTAMA;
	vTaskDelay(50);
	FIO1SET = LED_UTAMA;
	vTaskDelay(50);
	FIO1CLR = LED_UTAMA;
	vTaskDelay(50);
	FIO1SET = LED_UTAMA;
	pll_feed();
}

void vLedTask( void *pvParameters )	{
	struct t_env *st_env;
	st_env = ALMT_ENV;
	short cnt_astm = 0;
	FIO1SET = LED_UTAMA;
	char a=0, b=1, i=0;
	char ss[6];
	portTickType xLastWakeTime;
	const portTickType xFrequency = 500;
	int jml_mavg;
		
	vTaskDelay(100);
	//printf("  task : %s: %d\r\n", __FUNCTION__, uxTaskGetNumberOfTasks());

	st_hw.init++;
	do	{
		init_led();
	} while(st_hw.init != uxTaskGetNumberOfTasks());

	#ifdef MOV_AVG
	char toogle_mavg;
	//struct t_env *st_env;
	//st_env = ALMT_ENV;
	
	jml_mavg = st_env->n_mavg;
	toogle_mavg = 0;
	#endif
	
	#ifdef debug_ASTM
	int lok,lok2;
	lok = 0;
	lok2 = 20;
	//tes panggil nilai astm --> oke
	struct t_astm *st_astm;
	st_astm = pvPortMalloc(PER_ASTM * sizeof (struct t_astm) );
	if (st_astm == NULL)	{
		printf(" %s(): ERR allok memory gagal !\r\n", __FUNCTION__);
		vPortFree (st_astm);
		return 3;
	}

	memcpy((char *) st_astm, (char *) ALMT_VALUE_ASTM+(lok*JML_KOPI_ASTM), (PER_ASTM * sizeof (struct t_astm)));
	printf("|alpha = %f|", st_astm[0].koef);	
	
	memcpy((char *) st_astm, (char *) ALMT_VALUE_ASTM+(lok2*JML_KOPI_ASTM), (PER_ASTM * sizeof (struct t_astm)));
	printf("|omega = %f|\n\r", st_astm[90].koef);	
	vPortFree (st_astm);
	
	#endif
	
	int q = 0;
	xLastWakeTime = xTaskGetTickCount();
	for ( ;; )	{
		toogle_led_utama();
		//printf("testing %.4f\r\n", 12.3455*8.1);
		//printf("testing %d\r\n", 12);
		//qsprintf("perkalian %.4f\r\n", 12.328*3);
		//hitung_rpm();
		//data_frek_rpm();

		#ifdef PAKAI_ADC_7708
		data_adc();
		#endif
		
		#ifdef ADA_ASTM
		//counter untuk astm, periode 1 menit
		if (st_env->jumFlow){
			cnt_astm ++;
			if (cnt_astm == 120){ 
			cnt_astm = 0;
			astm_aktif = st_env->jumFlow; // nilai astm_aktif di isi dengan jumlah flowmeter dalam deret binary. ngertos??
			}
		}
		//printf("|%d|%d| ",cnt_astm,astm_aktif);
		#endif
		
		#ifdef MOV_AVG
		if (toogle_mavg) 
		{
			data_mavg(jml_mavg);
			toogle_mavg = 0;
		}
		else toogle_mavg = 1;
		#endif
		
		i = 1-i;
		if (i)	{
			stat_ps();
			pll_feed();
		}
		#if INCLUDE_vTaskDelayUntil==1
		vTaskDelayUntil( &xLastWakeTime, xFrequency );
		#else
		vTaskDelay(495);
		#endif
		st_hw.mm++;
		//uprintf("..%d", st_hw.mm);
	}
	
	#ifdef MOV_AVG
	vPortFree (st_mavg);
	#endif
}

/*-----------------------------------------------------------*/

static void prvSetupHardware( void )
{
#if 0
	#ifdef RUN_FROM_RAM
		/* Remap the interrupt vectors to RAM if we are are running from RAM. */
		SCB_MEMMAP = 2;
	#endif
	
	/* Disable the PLL. */
	PLLCON = 0;
	PLLFEED = mainPLL_FEED_BYTE1;
	PLLFEED = mainPLL_FEED_BYTE2;
	
	/* Configure clock source. */
	SCS |= mainOSC_ENABLE;
	while( !( SCS & mainOSC_STAT ) );
	CLKSRCSEL = mainOSC_SELECT; 
	
	/* Setup the PLL to multiply the XTAL input by 4. */
	PLLCFG = ( mainPLL_MUL | mainPLL_DIV );
	PLLFEED = mainPLL_FEED_BYTE1;
	PLLFEED = mainPLL_FEED_BYTE2;

	/* Turn on and wait for the PLL to lock... */
	PLLCON = mainPLL_ENABLE;
	PLLFEED = mainPLL_FEED_BYTE1;
	PLLFEED = mainPLL_FEED_BYTE2;
	CCLKCFG = mainCPU_CLK_DIV;	
	while( !( PLLSTAT & mainPLL_LOCK ) );
	
	/* Connecting the clock. */
	PLLCON = mainPLL_CONNECT;
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
	MAMCR = 0;
	MAMTIM = mainMAM_TIM_3;
	MAMCR = mainMAM_MODE_FULL;
	*/
	
	/* Setup the led's on the MCB2300 board */
	vParTestInitialise();
#endif
}







