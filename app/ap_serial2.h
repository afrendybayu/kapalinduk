
// Afrendy Bayu
// Depok, 25 Maret 2013
// 

/* Scheduler include files. */

//#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"

/* Demo program include files. */
#include "serial/serial.h"
#include "queue.h"
#include "hardware.h"


#define comSTACK_SIZE				configMINIMAL_STACK_SIZE



static xComPortHandle xPort2;
xTaskHandle *hdl_serial2;

void vAltStartCom2( unsigned portBASE_TYPE uxPriority, unsigned long ulBaudRate );
int proses_mod(int mbn, char *mbstr);
void init_banner2()	;
