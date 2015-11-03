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

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#include <stdio.h>
#include "lpc23xx.h"

/*-----------------------------------------------------------
 * Application specific definitions.
 *
 * These definitions should be adjusted for your particular hardware and
 * application requirements.
 *
 * THESE PARAMETERS ARE DESCRIBED WITHIN THE 'CONFIGURATION' SECTION OF THE
 * FreeRTOS API DOCUMENTATION AVAILABLE ON THE FreeRTOS.org WEB SITE. 
 *
 * See http://www.freertos.org/a00110.html.
 *----------------------------------------------------------*/


#define PROMPT				"iTong"
#define BOARD_SANTER		"Santer"
#define BOARD_SANTER_v1_0
//#define BOARD_SANTER_versi	"v1.2"
#define BOARD_SANTER_versi	"v1.3A"

#define PAKAI_LED_UTAMA
//#define PAKAI_RELAY
#define PAKAI_SHELL
#define printf 				printf0
//#define configCOMMAND_INT_MAX_OUTPUT_SIZE 1
#define PAKAI_TINYSH
#define PAKAI_RTC


#define ST_LED				3
#define ST_SHELL			20
#define ST_SANTER			8		//	10

//#define PAKAI_RELAY

#define PAKAI_SERIAL_2
#ifdef  PAKAI_SERIAL_2
	//#define PAKAI_SERIAL_2_P0		115200
	#define PAKAI_SERIAL_2_P0		9600
	#define ST_SER2					15
	#define PAKAI_MODBUS					// fungsi2 modbus di ap_utils.h
	#define MODBUS_RTU_SLAVE		
#endif

#define PAKAI_SERIAL_3
#ifdef  PAKAI_SERIAL_3
	//#define ERROR_DATA_RATE
	#define MOV_AVG
	//#define PAKAI_SERIAL_3_P4		115200
	#define PAKAI_SERIAL_3_P4		9600
	#define ST_SER3					25
	#define PAKAI_MODBUS					// fungsi2 modbus di ap_utils.h
	#define MODBUS_RTU_MASTER
#endif

#define PAKAI_SPI_SSP1
#ifdef  PAKAI_SPI_SSP1
	#define PAKAI_ADC_7708 
#endif


#define PAKAI_SPI_SSP0
#ifdef  PAKAI_SPI_SSP0
	#define PAKAI_FILE_SIMPAN
	#define PAKAI_SDCARD
#endif

/* Value to use on old rev '-' devices. */
#define configPINSEL2_VALUE	0x50151105

/* Value to use on rev 'A' and newer devices. */
//#define configPINSEL2_VALUE 	0x50150105

#ifndef configPINSEL2_VALUE
	#error Please uncomment one of the two configPINSEL2_VALUE definitions above, depending on the revision of the LPC2000 device being used.
#endif

//#define INCLUDE_uxTaskGetStackHighWaterMark	1

#define configUSE_PREEMPTION		1
#define configUSE_IDLE_HOOK         1
#define configUSE_TICK_HOOK         1
#define configCPU_CLOCK_HZ          ( ( unsigned long ) 60000000 )	/* =12Mhz xtal multiplied by 5 using the PLL. */
//#define configCPU_CLOCK_HZ          ( ( unsigned long ) 10000000 )//
//#define configCPU_CLOCK_HZ_low      ( ( unsigned long ) 10000000 )
#define configTICK_RATE_HZ          ( ( portTickType ) 1000 )
#define configMAX_PRIORITIES		( ( unsigned portBASE_TYPE ) 4 )
#define configMINIMAL_STACK_SIZE	( ( unsigned short ) 104 )
#define configTOTAL_HEAP_SIZE		( ( size_t ) ( 40 * 1024 ) )			// LPC2387, max:53 << bisa dicompile, tapi tidak jalan
																			// LPC2387, max:48
#define configMAX_TASK_NAME_LEN		( 16 )
#define configUSE_TRACE_FACILITY	1
#define configUSE_16_BIT_TICKS		0
#define configIDLE_SHOULD_YIELD		1
#define configUSE_MUTEXES			1

/* Co-routine definitions. */
#define configUSE_CO_ROUTINES 		0
#define configMAX_CO_ROUTINE_PRIORITIES ( 2 )


/* Set the following definitions to 1 to include the API function, or zero
to exclude the API function. */

#define INCLUDE_vTaskPrioritySet            0
#define INCLUDE_uxTaskPriorityGet           0
#define INCLUDE_vTaskDelete                 0
#define INCLUDE_vTaskCleanUpResources       0
#define INCLUDE_vTaskSuspend                1
#define INCLUDE_vTaskDelayUntil             1
#define INCLUDE_vTaskDelay                  1
#define INCLUDE_xTaskGetCurrentTaskHandle	0

//#define configGENERATE_RUN_TIME_STATS	1
//#define portCONFIGURE_TIMER_FOR_RUN_TIME_STATS() vConfigureTimerForRunTimeStats()
//#define portGET_RUN_TIME_COUNTER_VALUE() ulGetRunTimeCounterValue()


/* This demo makes use of one or more example stats formatting functions.  These
format the raw data provided by the uxTaskGetSystemState() function in to human
readable ASCII form.  See the notes in the implementation of vTaskList() within 
FreeRTOS/Source/tasks.c for limitations. */
#define configUSE_STATS_FORMATTING_FUNCTIONS	1


#endif /* FREERTOS_CONFIG_H */


#ifndef sbi
#define sbi(x,y)	x|=(1 << (y))
#endif

#ifndef cbi 
#define cbi(x,y)	x&=~(1 << (y))
#endif

#ifndef tstb
#define tstb(x,y)	(x & (1 << (y)) ? 1 : 0)
#endif

#ifndef toggle 
#define toggle(x,y)	x^=(1 << (y))
#endif

#ifndef BIT
#define BIT(x)	(1 << (x))

#endif
