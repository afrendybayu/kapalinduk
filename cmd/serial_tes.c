

#include "FreeRTOS.h"
#include "task.h"
#include <stdarg.h>

/* Demo program include files. */
#include "serial.h"
#include "queue.h"
#include "semphr.h"
#include "hardware.h"

xTaskHandle *hdl_shell;
static xComPortHandle xPort;

void uprintf(char *fmt, ...) {
//#ifdef PAKAI_SHELL
	char str_buffer[128];
	int lg=0;
    va_list args;
    va_start(args, fmt);
    lg = vsprintf(str_buffer, fmt, args);
    va_end(args);
    vSerialPutString(xPort, str_buffer, lg);
	vTaskDelay(5);
}

static portTASK_FUNCTION( shell, pvParameters )		{
signed char cExpectedByte, cByteRxed;
portBASE_TYPE xResyncRequired = pdFALSE, xErrorOccurred = pdFALSE;
portBASE_TYPE xGotChar;
int ch;
char s[30];

	/* Just to stop compiler warnings. */
	( void ) pvParameters;
	//vTaskDelay(1);
	//init_banner();
	
	vSerialPutString(xPort, "tes\r\n", 5);
	for(;;)	{
		uprintf("Merdeka!!!\r\n");
		vSerialPutString(xPort, "tes\r\n", 5);
		vTaskDelay(1000);
	}
}


void serialTask( unsigned portBASE_TYPE uxPriority, unsigned long ulBaudRate )		{
//const unsigned portBASE_TYPE uxQueueSize = 10;
//const unsigned portBASE_TYPE uxQueueLength = 128;
	//xPrintQueue = xQueueCreate( uxQueueSize, uxQueueLength );
	/* Initialise the com port then spawn the Rx and Tx tasks. */
	xSerialPortInitMinimal( ulBaudRate, configMINIMAL_STACK_SIZE );
	
	/* The Tx task is spawned with a lower priority than the Rx task. */
	// uxPriority = uxPriority
	xTaskCreate( shell, ( signed char * ) "Shell", configMINIMAL_STACK_SIZE * ST_SHELL, NULL, tskIDLE_PRIORITY, ( xTaskHandle * ) hdl_shell );
}
