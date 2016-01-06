
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
#define nRespM 			160

#define comSTACK_SIZE				configMINIMAL_STACK_SIZE

#ifdef PAKAI_SERIAL_1
int cek_AT_cmd();
int Echo_off_cmd();
#endif



static xComPortHandle xPort1;
xTaskHandle *hdl_serial1;

void vAltStartCom1( unsigned portBASE_TYPE uxPriority, unsigned long ulBaudRate );
//int proses_mod(int mbn, char *mbstr);
//void init_banner1()	;
