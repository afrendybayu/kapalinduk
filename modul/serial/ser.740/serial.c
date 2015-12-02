/*
    FreeRTOS V7.4.0 - Copyright (C) 2013 Real Time Engineers Ltd.

    FEATURES AND PORTS ARE ADDED TO FREERTOS ALL THE TIME.  PLEASE VISIT
    http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS tutorial books are available in pdf and paperback.        *
     *    Complete, revised, and edited pdf reference manuals are also       *
     *    available.                                                         *
     *                                                                       *
     *    Purchasing FreeRTOS documentation will not only help you, by       *
     *    ensuring you get running as quickly as possible and with an        *
     *    in-depth knowledge of how to use FreeRTOS, it will also help       *
     *    the FreeRTOS project to continue with its mission of providing     *
     *    professional grade, cross platform, de facto standard solutions    *
     *    for microcontrollers - completely free of charge!                  *
     *                                                                       *
     *    >>> See http://www.FreeRTOS.org/Documentation for details. <<<     *
     *                                                                       *
     *    Thank you for using FreeRTOS, and thank you for your support!      *
     *                                                                       *
    ***************************************************************************


    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation AND MODIFIED BY the FreeRTOS exception.

    >>>>>>NOTE<<<<<< The modification to the GPL is included to allow you to
    distribute a combined work that includes FreeRTOS without being obliged to
    provide the source code for proprietary components outside of the FreeRTOS
    kernel.

    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details. You should have received a copy of the GNU General Public License
    and the FreeRTOS license exception along with FreeRTOS; if not itcan be
    viewed here: http://www.freertos.org/a00114.html and also obtained by
    writing to Real Time Engineers Ltd., contact details for whom are available
    on the FreeRTOS WEB site.

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
    including FreeRTOS+Trace - an indispensable productivity tool, and our new
    fully thread aware and reentrant UDP/IP stack.

    http://www.OpenRTOS.com - Real Time Engineers ltd license FreeRTOS to High 
    Integrity Systems, who sell the code with commercial support, 
    indemnification and middleware, under the OpenRTOS brand.
    
    http://www.SafeRTOS.com - High Integrity Systems also provide a safety 
    engineered and independently SIL3 certified version for use in safety and 
    mission critical applications that require provable dependability.
*/

/*
	Changes from V2.4.0

		+ Made serial ISR handling more complete and robust.

	Changes from V2.4.1

		+ Split serial.c into serial.c and serialISR.c.  serial.c can be 
		  compiled using ARM or THUMB modes.  serialISR.c must always be
		  compiled in ARM mode.
		+ Another small change to cSerialPutChar().

	Changed from V2.5.1

		+ In cSerialPutChar() an extra check is made to ensure the post to
		  the queue was successful if then attempting to retrieve the posted
		  character.

*/

/* 
	BASIC INTERRUPT DRIVEN SERIAL PORT DRIVER FOR UART0. 

	This file contains all the serial port components that can be compiled to
	either ARM or THUMB mode.  Components that must be compiled to ARM mode are
	contained in serialISR.c.
*/

/* Standard includes. */
#include <stdlib.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

/* Demo application includes. */
#include "serial.h"

/*-----------------------------------------------------------*/

/* Constants to setup and access the UART. */
#define serDLAB							( ( unsigned char ) 0x80 )
#define serENABLE_INTERRUPTS			( ( unsigned char ) 0x03 )
#define serNO_PARITY					( ( unsigned char ) 0x00 )
#define ser1_STOP_BIT					( ( unsigned char ) 0x00 )
#define ser8_BIT_CHARS					( ( unsigned char ) 0x03 )
#define serFIFO_ON						( ( unsigned char ) 0x01 )
#define serCLEAR_FIFO					( ( unsigned char ) 0x06 )
#define serWANTED_CLOCK_SCALING			( ( unsigned long ) 16 )

/* Constants to setup and access the VIC. */
#define serUART0_VIC_CHANNEL			( ( unsigned long ) 0x0006 )
#define serUART0_VIC_CHANNEL_BIT		( ( unsigned long ) 0x0040 )
#define serUART0_VIC_ENABLE				( ( unsigned long ) 0x0020 )
#define serCLEAR_VIC_INTERRUPT			( ( unsigned long ) 0 )

#define serUART2_VIC_CHANNEL_BIT		( ( unsigned long ) 0x0040 )
#define serUART2_VIC_ENABLE				( ( unsigned long ) 0x0020 )


#define serINVALID_QUEUE				( ( xQueueHandle ) 0 )
#define serHANDLE						( ( xComPortHandle ) 1 )
#define serNO_BLOCK						( ( portTickType ) 0 )

/*-----------------------------------------------------------*/

#include <stdarg.h>


int printf0 (const char *fmt, ...)		{
	va_list args;
	int i;
	char printbuffer[256];
	va_start (args, fmt);

	i = vsprintf (printbuffer, fmt, args);
	va_end (args);

	/* Print the string */
	vSerialPutString(1, printbuffer, 256);
	return 0;
} 


#ifdef PAKAI_SERIAL_2

static volatile long *plTHREEmpty2;
static xQueueHandle xRxedChars2; 
static xQueueHandle xCharsForTx2;

xComPortHandle xSerialPortInit2( unsigned long ulWantedBaud, unsigned portBASE_TYPE uxQueueLength )	{
unsigned long ulDivisor, ulWantedClock;
xComPortHandle xReturn = serHANDLE;
extern void ( vUART2_ISR_Wrapper )( void );

	/* The queues are used in the serial ISR routine, so are created from
	serialISR.c (which is always compiled to ARM mode. */
	vSerialISRCreateQueues2( uxQueueLength, &xRxedChars2, &xCharsForTx2, &plTHREEmpty2 );

	if( 
		( xRxedChars2 != serINVALID_QUEUE ) && 
		( xCharsForTx2 != serINVALID_QUEUE ) && 
		( ulWantedBaud != ( unsigned long ) 0 ) 
	  )
	{
		portENTER_CRITICAL();
		{
			/* Setup the baud rate:  Calculate the divisor value. */
			ulWantedClock = ulWantedBaud * serWANTED_CLOCK_SCALING;
			ulDivisor = configCPU_CLOCK_HZ / ulWantedClock;

			/* Set the DLAB bit so we can access the divisor. */
			U2LCR |= serDLAB;

			/* Setup the divisor. */
			U2DLL = ( unsigned char ) ( ulDivisor & ( unsigned long ) 0xff );
			ulDivisor >>= 8;
			U2DLM = ( unsigned char ) ( ulDivisor & ( unsigned long ) 0xff );

			/* Turn on the FIFO's and clear the buffers. */
			U2FCR = ( serFIFO_ON | serCLEAR_FIFO );

			/* Setup transmission format. */
			U2LCR = serNO_PARITY | ser1_STOP_BIT | ser8_BIT_CHARS;

			/* Setup the VIC for the UART. */
			VICIntSelect &= ~( serUART2_VIC_CHANNEL_BIT );
			VICIntEnable |= serUART2_VIC_CHANNEL_BIT;
			VICVectAddr28 = ( long ) vUART2_ISR_Wrapper;
			//VICVectCntl28 = serUART_VIC_ENABLE | serUART_VIC_CHANNEL;
			VICVectCntl28 = serUART2_VIC_ENABLE | 28;

			/* Enable UART2 interrupts. */
			U2IER |= serENABLE_INTERRUPTS;
		}
		portEXIT_CRITICAL();
	}
	else
	{
		xReturn = ( xComPortHandle ) 0;
	}

	return xReturn;
}

signed portBASE_TYPE xSerialGetChar2( xComPortHandle pxPort, signed char *pcRxedChar, portTickType xBlockTime )
{
	/* The port handle is not required as this driver only supports UART0. */
	( void ) pxPort;

	/* Get the next character from the buffer.  Return false if no characters
	are available, or arrive before xBlockTime expires. */
	if( xQueueReceive( xRxedChars2, pcRxedChar, xBlockTime ) )
	{
		return pdTRUE;
	}
	else
	{
		return pdFALSE;
	}
}

void vSerialPutString2( xComPortHandle pxPort, const signed char * const pcString, unsigned short usStringLength )
{
signed char *pxNext;

	/* NOTE: This implementation does not handle the queue being full as no
	block time is used! */

	/* The port handle is not required as this driver only supports UART0. */
	( void ) pxPort;
	( void ) usStringLength;

	/* Send each character in the string, one at a time. */
	pxNext = ( signed char * ) pcString;
	while( *pxNext )
	{
		xSerialPutChar2( pxPort, *pxNext, serNO_BLOCK );
		//xSerialPutChar( pxPort, *pxNext, 10000 );		// kalau 
		pxNext++;
	}
}

signed portBASE_TYPE xSerialPutChar2( xComPortHandle pxPort, signed char cOutChar, portTickType xBlockTime )
{
signed portBASE_TYPE xReturn;

	/* This demo driver only supports one port so the parameter is not used. */
	( void ) pxPort;

	portENTER_CRITICAL();
	{
		/* Is there space to write directly to the UART? */
		if( *plTHREEmpty2 == ( long ) pdTRUE )
		{
			/* We wrote the character directly to the UART, so was 
			successful. */
			*plTHREEmpty2 = pdFALSE;
			U2THR = cOutChar;
			xReturn = pdPASS;
		}
		else 
		{
			/* We cannot write directly to the UART, so queue the character.
			Block for a maximum of xBlockTime if there is no space in the
			queue. */
			xReturn = xQueueSend( xCharsForTx2, &cOutChar, xBlockTime );

			/* Depending on queue sizing and task prioritisation:  While we 
			were blocked waiting to post interrupts were not disabled.  It is 
			possible that the serial ISR has emptied the Tx queue, in which
			case we need to start the Tx off again. */
			if( ( *plTHREEmpty2 == ( long ) pdTRUE ) && ( xReturn == pdPASS ) )
			{
				xQueueReceive( xCharsForTx2, &cOutChar, serNO_BLOCK );
				*plTHREEmpty2 = pdFALSE;
				U2THR = cOutChar;
			}
		}
	}
	portEXIT_CRITICAL();

	return xReturn;
}

#endif

//#ifdef PAKAI_SHELL
#if 1

/* Queues used to hold received characters, and characters waiting to be
transmitted. */
static xQueueHandle xRxedChars; 
static xQueueHandle xCharsForTx; 

/*-----------------------------------------------------------*/

/* Communication flag between the interrupt service routine and serial API. */
static volatile long *plTHREEmpty;

/* 
 * The queues are created in serialISR.c as they are used from the ISR.
 * Obtain references to the queues and THRE Empty flag. 
 */
extern void vSerialISRCreateQueues(	unsigned portBASE_TYPE uxQueueLength, xQueueHandle *pxRxedChars, xQueueHandle *pxCharsForTx, long volatile **pplTHREEmptyFlag );

/*-----------------------------------------------------------*/

xComPortHandle xSerialPortInitMinimal( unsigned long ulWantedBaud, unsigned portBASE_TYPE uxQueueLength )	{
unsigned long ulDivisor, ulWantedClock;
xComPortHandle xReturn = serHANDLE;
extern void ( vUART_ISR_Wrapper )( void );

	/* The queues are used in the serial ISR routine, so are created from
	serialISR.c (which is always compiled to ARM mode. */
	vSerialISRCreateQueues( uxQueueLength, &xRxedChars, &xCharsForTx, &plTHREEmpty );

	if( 
		( xRxedChars != serINVALID_QUEUE ) && 
		( xCharsForTx != serINVALID_QUEUE ) && 
		( ulWantedBaud != ( unsigned long ) 0 ) 
	  )
	{
		portENTER_CRITICAL();
		{
			/* Setup the baud rate:  Calculate the divisor value. */
			ulWantedClock = ulWantedBaud * serWANTED_CLOCK_SCALING;
			ulDivisor = configCPU_CLOCK_HZ / ulWantedClock;

			/* Set the DLAB bit so we can access the divisor. */
			UART0_LCR |= serDLAB;

			/* Setup the divisor. */
			UART0_DLL = ( unsigned char ) ( ulDivisor & ( unsigned long ) 0xff );
			ulDivisor >>= 8;
			UART0_DLM = ( unsigned char ) ( ulDivisor & ( unsigned long ) 0xff );

			/* Turn on the FIFO's and clear the buffers. */
			UART0_FCR = ( serFIFO_ON | serCLEAR_FIFO );

			/* Setup transmission format. */
			UART0_LCR = serNO_PARITY | ser1_STOP_BIT | ser8_BIT_CHARS;

			/* Setup the VIC for the UART. */
			VICIntSelect &= ~( serUART0_VIC_CHANNEL_BIT );
			VICIntEnable |= serUART0_VIC_CHANNEL_BIT;
			VICVectAddr6 = ( long ) vUART_ISR_Wrapper;
			VICVectCntl6 = serUART0_VIC_ENABLE | serUART0_VIC_CHANNEL;

			/* Enable UART0 interrupts. */
			UART0_IER |= serENABLE_INTERRUPTS;
		}
		portEXIT_CRITICAL();
	}
	else
	{
		xReturn = ( xComPortHandle ) 0;
	}

	return xReturn;
}
/*-----------------------------------------------------------*/

signed portBASE_TYPE xSerialGetChar( xComPortHandle pxPort, signed char *pcRxedChar, portTickType xBlockTime )
{
	/* The port handle is not required as this driver only supports UART0. */
	( void ) pxPort;

	/* Get the next character from the buffer.  Return false if no characters
	are available, or arrive before xBlockTime expires. */
	if( xQueueReceive( xRxedChars, pcRxedChar, xBlockTime ) )
	{
		return pdTRUE;
	}
	else
	{
		return pdFALSE;
	}
}
/*-----------------------------------------------------------*/

void vSerialPutString( xComPortHandle pxPort, const signed char * const pcString, unsigned short usStringLength )
{
signed char *pxNext;

	/* NOTE: This implementation does not handle the queue being full as no
	block time is used! */

	/* The port handle is not required as this driver only supports UART0. */
	( void ) pxPort;
	( void ) usStringLength;

	/* Send each character in the string, one at a time. */
	pxNext = ( signed char * ) pcString;
	while( *pxNext )
	{
		xSerialPutChar( pxPort, *pxNext, serNO_BLOCK );
		//xSerialPutChar( pxPort, *pxNext, 10000 );		// kalau 
		pxNext++;
	}
}
/*-----------------------------------------------------------*/

signed portBASE_TYPE xSerialPutChar( xComPortHandle pxPort, signed char cOutChar, portTickType xBlockTime )
{
signed portBASE_TYPE xReturn;

	/* This demo driver only supports one port so the parameter is not used. */
	( void ) pxPort;

	portENTER_CRITICAL();
	{
		/* Is there space to write directly to the UART? */
		if( *plTHREEmpty == ( long ) pdTRUE )
		{
			/* We wrote the character directly to the UART, so was 
			successful. */
			*plTHREEmpty = pdFALSE;
			UART0_THR = cOutChar;
			xReturn = pdPASS;
		}
		else 
		{
			/* We cannot write directly to the UART, so queue the character.
			Block for a maximum of xBlockTime if there is no space in the
			queue. */
			xReturn = xQueueSend( xCharsForTx, &cOutChar, xBlockTime );

			/* Depending on queue sizing and task prioritisation:  While we 
			were blocked waiting to post interrupts were not disabled.  It is 
			possible that the serial ISR has emptied the Tx queue, in which
			case we need to start the Tx off again. */
			if( ( *plTHREEmpty == ( long ) pdTRUE ) && ( xReturn == pdPASS ) )
			{
				xQueueReceive( xCharsForTx, &cOutChar, serNO_BLOCK );
				*plTHREEmpty = pdFALSE;
				UART0_THR = cOutChar;
			}
		}
	}
	portEXIT_CRITICAL();

	return xReturn;
}
/*-----------------------------------------------------------*/

void vSerialClose( xComPortHandle xPort )
{
	/* Not supported as not required by the demo application. */
	( void ) xPort;
}
/*-----------------------------------------------------------*/

#endif
