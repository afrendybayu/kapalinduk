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

#define serINVALID_QUEUE				( ( xQueueHandle ) 0 )
#define serHANDLE						( ( xComPortHandle ) 1 )
#define serNO_BLOCK						( ( portTickType ) 0 )

/*-----------------------------------------------------------*/

#ifdef PAKAI_SERIAL_3


static xQueueHandle Qrx3;
static xQueueHandle Qtx3;
static volatile portLONG *plTHREEmpty3;

#define serUART3_VIC_CHANNEL			( ( unsigned portLONG ) 0x0006 )
#define serUART3_VIC_CHANNEL_BIT		( ( unsigned portLONG ) 0x0040 )
#define serUART3_VIC_ENABLE				( ( unsigned portLONG ) 0x0020 )

extern void vSerialISRCreateQueues3(	unsigned portBASE_TYPE uxQueueLength, xQueueHandle *pxRxedChars, xQueueHandle *pxCharsForTx, long volatile **pplTHREEmptyFlag );

xComPortHandle xSerialPortInit3( unsigned portLONG ulWantedBaud, unsigned portBASE_TYPE uxQueueLength )	{
	unsigned portLONG ulDivisor, ulWantedClock;
	xComPortHandle xReturn = serHANDLE;
	extern void ( vUART3_ISR_Wrapper )( void );
	
	//printf("%s(): ", __FUNCTION__);
	
	/* The queues are used in the serial ISR routine, so are created from
	serialISR.c (which is always compiled to ARM mode. */
	vSerialISRCreateQueues3( uxQueueLength*8, &Qrx3, &Qtx3, &plTHREEmpty3 );
	
	if(
		( Qrx3 != serINVALID_QUEUE ) &&
		( Qtx3 != serINVALID_QUEUE ) &&
		( ulWantedBaud != ( unsigned portLONG ) 0 )
	  )
	{
		//printf(" buf OK\r\n");
		portENTER_CRITICAL();
		{
			/* Setup the baud rate:  Calculate the divisor value. */
			ulWantedClock = ulWantedBaud * serWANTED_CLOCK_SCALING;
			ulDivisor = configCPU_CLOCK_HZ / ulWantedClock;
			/* Set the DLAB bit so we can access the divisor. */
			U3LCR |= serDLAB;
			/* Setup the divisor. */
			U3DLL = ( unsigned portCHAR ) ( ulDivisor & ( unsigned portLONG ) 0xff );
			ulDivisor >>= 8;
			U3DLM = ( unsigned portCHAR ) ( ulDivisor & ( unsigned portLONG ) 0xff );
			/* Turn on the FIFO's and clear the buffers. */
			U3FCR = ( serFIFO_ON | serCLEAR_FIFO );
			/* Setup transmission format. */
			U3LCR = serNO_PARITY | ser1_STOP_BIT | ser8_BIT_CHARS;
			//#define serUART0_VIC_CHANNEL			( ( unsigned portLONG ) 0x0006 )
			//#define serUART0_VIC_CHANNEL_BIT		( ( unsigned portLONG ) 0x0040 )
			//#define serUART0_VIC_ENABLE				( ( unsigned portLONG ) 0x0020 )

			/* Setup the VIC for the UART. */
			//VICIntSelect &= ~( serUART0_VIC_CHANNEL_BIT );
			
			#if 1
			VICIntSelect &= ~BIT(29);
			VICVectAddr29 = ( portLONG ) vUART3_ISR_Wrapper;
			VICVectCntl29 = (serUART3_VIC_ENABLE | 29);
		
			/* Enable UART2 interrupts. */
			U3IER |= serENABLE_INTERRUPTS;
			
			VICIntEnable |= BIT(29);
			#endif
			
		}
		portEXIT_CRITICAL();
	}
	else
	{
		//printf(" Tidak bisa init !!\r\n");
		xReturn = ( xComPortHandle ) 0;
	}
	return xReturn;
}

signed portBASE_TYPE xSerialGetChar3( xComPortHandle pxPort3, signed portCHAR *pcRxedChar, portTickType xBlockTime )
{
	/* The port handle is not required as this driver only supports UART0. */
	( void ) pxPort3;
	/* Get the next character from the buffer.  Return false if no characters
	are available, or arrive before xBlockTime expires. */
	if( xQueueReceive( Qrx3, pcRxedChar, xBlockTime ) )
	{
		return pdTRUE;
	}
	else
	{
		return pdFALSE;
	}
}


signed portBASE_TYPE xSerialPutChar3( xComPortHandle pxPort3, signed portCHAR cOutChar, portTickType xBlockTime )
{
	signed portBASE_TYPE xReturn;
	/* This demo driver only supports one port so the parameter is not used. */
	( void ) pxPort3;
	
	portENTER_CRITICAL();
	{
		/* Is there space to write directly to the UART? */
		if( *plTHREEmpty3 == ( portLONG ) pdTRUE )
		{
			//printf2(": kosong\r\n");
			
			//* We wrote the character directly to the UART, so was
			//successful. 
			*plTHREEmpty3 = pdFALSE;
			U3THR = cOutChar;
			xReturn = pdPASS;
		}
		else
		{
			//printf2("%s(): aktif\r\n", __FUNCTION__);
			/* We cannot write directly to the UART, so queue the character.
			Block for a maximum of xBlockTime if there is no space in the
			queue. */
			xReturn = xQueueSend( Qtx3, &cOutChar, xBlockTime );
			
			/*
			if (xReturn == pdPASS)
				printf2("%s(): bs masuk\r\n", __FUNCTION__);
			else
				printf2("%s(): masih penuh\r\n", __FUNCTION__);
			*/
			
			#if 1
			/* Depending on queue sizing and task prioritisation:  While we
			were blocked waiting to post interrupts were not disabled.  It is
			possible that the serial ISR has emptied the Tx queue, in which
			case we need to start the Tx off again. */
			if( ( *plTHREEmpty3 == ( portLONG ) pdTRUE ) && ( xReturn == pdPASS ) )
			{
				xQueueReceive( Qtx3, &cOutChar, serNO_BLOCK );
				*plTHREEmpty3 = pdFALSE;
				U3THR = cOutChar;
			}
			#endif
		}
	}
	portEXIT_CRITICAL();	
	return xReturn;
}

void vSerialPutString3( xComPortHandle pxPort3, const signed char * const pcString, unsigned short usStringLength )	{
	signed portCHAR *pxNext;
	/* Send each character in the string, one at a time. */
	pxNext = ( signed portCHAR * ) pcString;
	while( *pxNext )
	{	
		//xSerialPutChar2 (pxPort2, *pxNext, serNO_BLOCK);
		xSerialPutChar3 (pxPort3, *pxNext, 256);
		//xSerialPutChar2( 1, *pxNext, 1000 );	// 100 OK
		pxNext++;
	}
}


#endif


#ifdef PAKAI_SERIAL_2


static xQueueHandle Qrx2;
static xQueueHandle Qtx2;
static volatile portLONG *plTHREEmpty2;

#define serUART2_VIC_CHANNEL			( ( unsigned portLONG ) 0x0006 )
#define serUART2_VIC_CHANNEL_BIT		( ( unsigned portLONG ) 0x0040 )
#define serUART2_VIC_ENABLE				( ( unsigned portLONG ) 0x0020 )

extern void vSerialISRCreateQueues2(	unsigned portBASE_TYPE uxQueueLength, xQueueHandle *pxRxedChars, xQueueHandle *pxCharsForTx, long volatile **pplTHREEmptyFlag );

xComPortHandle xSerialPortInit2( unsigned portLONG ulWantedBaud, unsigned portBASE_TYPE uxQueueLength )	{
	unsigned portLONG ulDivisor, ulWantedClock;
	xComPortHandle xReturn = serHANDLE;
	extern void ( vUART2_ISR_Wrapper )( void );
	
	//printf("%s(): ", __FUNCTION__);
	
	/* The queues are used in the serial ISR routine, so are created from
	serialISR.c (which is always compiled to ARM mode. */
	vSerialISRCreateQueues2( uxQueueLength*8, &Qrx2, &Qtx2, &plTHREEmpty2 );
	
	if(
		( Qrx2 != serINVALID_QUEUE ) &&
		( Qtx2 != serINVALID_QUEUE ) &&
		( ulWantedBaud != ( unsigned portLONG ) 0 )
	  )
	{
		//printf(" buf OK\r\n");
		portENTER_CRITICAL();
		{
			/* Setup the baud rate:  Calculate the divisor value. */
			ulWantedClock = ulWantedBaud * serWANTED_CLOCK_SCALING;
			ulDivisor = configCPU_CLOCK_HZ / ulWantedClock;
			/* Set the DLAB bit so we can access the divisor. */
			U2LCR |= serDLAB;
			/* Setup the divisor. */
			U2DLL = ( unsigned portCHAR ) ( ulDivisor & ( unsigned portLONG ) 0xff );
			ulDivisor >>= 8;
			U2DLM = ( unsigned portCHAR ) ( ulDivisor & ( unsigned portLONG ) 0xff );
			/* Turn on the FIFO's and clear the buffers. */
			U2FCR = ( serFIFO_ON | serCLEAR_FIFO );
			/* Setup transmission format. */
			U2LCR = serNO_PARITY | ser1_STOP_BIT | ser8_BIT_CHARS;
			//#define serUART0_VIC_CHANNEL			( ( unsigned portLONG ) 0x0006 )
			//#define serUART0_VIC_CHANNEL_BIT		( ( unsigned portLONG ) 0x0040 )
			//#define serUART0_VIC_ENABLE				( ( unsigned portLONG ) 0x0020 )

			/* Setup the VIC for the UART. */
			//VICIntSelect &= ~( serUART0_VIC_CHANNEL_BIT );
			
			#if 1
			VICIntSelect &= ~BIT(28);
			VICVectAddr28 = ( portLONG ) vUART2_ISR_Wrapper;
			VICVectCntl28 = (serUART0_VIC_ENABLE | 28);
		
			/* Enable UART2 interrupts. */
			U2IER |= serENABLE_INTERRUPTS;
			
			VICIntEnable |= BIT(28);
			#endif
			
		}
		portEXIT_CRITICAL();
	}
	else
	{
		//printf(" Tidak bisa init !!\r\n");
		xReturn = ( xComPortHandle ) 0;
	}
	return xReturn;
}

signed portBASE_TYPE xSerialGetChar2( xComPortHandle pxPort2, signed portCHAR *pcRxedChar, portTickType xBlockTime )
{
	/* The port handle is not required as this driver only supports UART0. */
	( void ) pxPort2;
	/* Get the next character from the buffer.  Return false if no characters
	are available, or arrive before xBlockTime expires. */
	if( xQueueReceive( Qrx2, pcRxedChar, xBlockTime ) )
	{
		return pdTRUE;
	}
	else
	{
		return pdFALSE;
	}
}

void vSerialPutString2( xComPortHandle pxPort2, const signed char * const pcString, unsigned short usStringLength )	{
	signed portCHAR *pxNext;
	/* Send each character in the string, one at a time. */
	pxNext = ( signed portCHAR * ) pcString;
	while( *pxNext )
	{	
		//xSerialPutChar2 (pxPort2, *pxNext, serNO_BLOCK);
		xSerialPutChar2 (pxPort2, *pxNext, 256);
		//xSerialPutChar2( 1, *pxNext, 1000 );	// 100 OK
		pxNext++;
	}
}

signed portBASE_TYPE xSerialPutChar2( xComPortHandle pxPort2, signed portCHAR cOutChar, portTickType xBlockTime )
{
	signed portBASE_TYPE xReturn;
	/* This demo driver only supports one port so the parameter is not used. */
	( void ) pxPort2;
	
	portENTER_CRITICAL();
	{
		/* Is there space to write directly to the UART? */
		if( *plTHREEmpty2 == ( portLONG ) pdTRUE )
		{
			//printf2(": kosong\r\n");
			
			//* We wrote the character directly to the UART, so was
			//successful. 
			*plTHREEmpty2 = pdFALSE;
			U2THR = cOutChar;
			xReturn = pdPASS;
		}
		else
		{
			//printf2("%s(): aktif\r\n", __FUNCTION__);
			/* We cannot write directly to the UART, so queue the character.
			Block for a maximum of xBlockTime if there is no space in the
			queue. */
			xReturn = xQueueSend( Qtx2, &cOutChar, xBlockTime );
			
			/*
			if (xReturn == pdPASS)
				printf2("%s(): bs masuk\r\n", __FUNCTION__);
			else
				printf2("%s(): masih penuh\r\n", __FUNCTION__);
			*/
			
			#if 1
			/* Depending on queue sizing and task prioritisation:  While we
			were blocked waiting to post interrupts were not disabled.  It is
			possible that the serial ISR has emptied the Tx queue, in which
			case we need to start the Tx off again. */
			if( ( *plTHREEmpty2 == ( portLONG ) pdTRUE ) && ( xReturn == pdPASS ) )
			{
				xQueueReceive( Qtx2, &cOutChar, serNO_BLOCK );
				*plTHREEmpty2 = pdFALSE;
				U2THR = cOutChar;
			}
			#endif
		}
	}
	portEXIT_CRITICAL();	
	return xReturn;
}

#endif

#ifdef PAKAI_SHELL
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

xComPortHandle xSerialPortInitMinimal( unsigned long ulWantedBaud, unsigned portBASE_TYPE uxQueueLength )
{
unsigned long ulDivisor, ulWantedClock;
xComPortHandle xReturn = serHANDLE;
extern void ( vUART_ISR_Wrapper )( void );

	/* The queues are used in the serial ISR routine, so are created from
	serialISR.c (which is always compiled to ARM mode. */
	vSerialISRCreateQueues( uxQueueLength*8, &xRxedChars, &xCharsForTx, &plTHREEmpty );

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
			VICVectCntl6 = serUART0_VIC_CHANNEL | serUART0_VIC_ENABLE;

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
