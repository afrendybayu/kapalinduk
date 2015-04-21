
// Afrendy Bayu
// Depok, 25 Maret 2013
// 

/* Scheduler include files. */


#include "FreeRTOS.h"
#include "task.h"

/* Demo program include files. */
#include "serial/serial.h"
#include "modbus/mb.h"
#include "ap_serial3.h"
//#include "sh_serial.h"
//#include <stdarg.h>
#include "queue.h"
#include "hardware.h"
#include "monita.h"
#include <stdarg.h>
//#include "sh_hardware.h"

#ifdef PAKAI_SERIAL_3

static xComPortHandle xPort3;//	__attribute__ ((section (".usbram1")));
//extern volatile struct t_st_hw st_hw;

//void sedot_mod(int ch);
//int proses_mod_cmd();

//extern char strmb[];

//int parsing_mod(unsigned char *x);
char strmb3[MAX_RX_MB3]		__attribute__ ((section (".usbram1")));
char outmb3[MAX_RX_MB3]		__attribute__ ((section (".usbram1")));

#if 0
static xQueueHandle xPrintQueue2;

// qsprintf : custom printf yg dapat menyimpan data ke queue
void qsprintf2(char *fmt, ...) {
	//uprintf("---> KIRIM : ");
	char str_buffer[256];
	int lg=0;
	va_list args;
	va_start(args, fmt);
	lg = vsprintf(str_buffer, fmt, args);
	lg = xQueueSend( xPrintQueue2, ( void * ) str_buffer, ( portTickType ) 0 );
	va_end(args);

}
#endif

void printd3(int prio, const char *format, ...)	{
	va_list arg;
	int lg=0;
	char str_buffer[256];
	if (prio>0)	{
	//if (prio>env.prioDebug2)	{
		va_start (arg, format);
		lg = vsprintf (str_buffer, format, arg);	
		va_end (arg);
		//printf("%s", format);
		enaTX3_485();
		vSerialPutString3(xPort3, format, lg);
		vTaskDelay(10);
		disTX3_485();
	}
}



static portTASK_FUNCTION( vComTask3, pvParameters )		{
signed char cExpectedByte, cByteRxed;
portBASE_TYPE xResyncRequired = pdFALSE, xErrorOccurred = pdFALSE;
portBASE_TYPE xGotChar;
int ch;
char s[30];
	//char strmb[MAX_RX_MB];
	int  nmb = 0, balas = 0;
	char flag_ms = 0;
	/* Just to stop compiler warnings. */
	( void ) pvParameters;
	vTaskDelay(100);
	init_banner3();
	
	vTaskDelay(500);
	st_hw.init++;
	//nSer2 = 0;
	int loop2 = 0;
	//disTX3_485();
	enaTX3_485();
	enaRX3_485();
	printf("sampe sini 111\r\n");
	do {
		vTaskDelay(100);
		vSerialPutString3(xPort3, "xerial3\r\n", 6);
	} while(st_hw.init != uxTaskGetNumberOfTasks());
		
	printf("sampe sini 222\r\n");
	for( ;; )	{
		//printd2(10, "serial 2: %d\r\n", loop2++);
		printf("sampe sini 3333\r\n");
		//printd3(10, "___serial 3\r\n");
		vSerialPutString3(xPort3, "tes3\r\n", 6);
		//vTaskDelay(1000);
		/*
		xGotChar = xSerialGetChar3( xPort3, &ch, 10 );
		if( xGotChar == pdTRUE )		{
			if ((nmb==0) && ((char)ch==0xff))	{
				//printf("nmb: %d, ch: %02x, mask sini !!", nmb, (char)ch);
			} 
			else {
				//printf("%02x ", (char) ch);
				//printf("%c ", (char) ch);
				strmb3[nmb] = (char) ch;
				nmb++;
				//strSer2[nmb+1] = '\0';
				//sedot_mod(ch);
				flag_ms=1;
			}
		}
		else {
			// sedot data respon (sendiri), clear buffer
			if ( (balas==nmb) && (balas>0) )	{			
				//printf("Reset MB2 !!!\r\n");
				nmb = 0;
				flag_ms = 0;
				balas = 0;
			}
			
			if (flag_ms==1 && nmb>4)	{
				balas = proses_mod3(nmb, strmb3);
				//printf("--==> BALAS MB: %d\r\n", balas);
				nmb = 0;
			}
			if (balas==0)	{
				nmb = 0;
			}
			//
			#if 0
			flag_ms = 0;
			nmb = 0;
			#endif
		}
		//*/
	}
}

int proses_mod3(int mbn, char *mbstr)	{
	int hsl=0, cmd=0, jml=0, reg=0;

	#if 0
	printf("\r\nJml CMD: %d -->", mbn);
	int i,mm;
	
	mm = (mbn>40)?40:mbn;
	//for (i=0; i<mbn; i++)		{
	for (i=0; i<mm; i++)		{
		//printf(" %02x", mbstr[i]);
		printf(" %02x", strmb3[i]);
	}
	printf("\r\n");
	#endif
	
	struct t_env *p_env3;
	p_env3 = (char *) ALMT_ENV;
		
	//if (p_env3->almtSlave != mbstr[0])	{
	if (p_env3->almtSlave != strmb3[0])	{
		//printf("0: %02x\r\n", strmb[0]);
		return 1;
	}
	
	//cmd = mbstr[1];
	cmd = strmb3[1];
	if (cmd==READ_FILE_CONTENT && mbn>8)	return 3;		// kiriman sendiri
	
	/*
	if (cmd>=READ_FILE_CONTENT)
		//hsl = cek_crc_ccitt_0xffff(mbn, mbstr);		// modbus-serial dari skywave data file
		hsl = cek_crc_ccitt_0xffff(mbn, strmb3);			//
	else
		//hsl = cek_crc_mod(mbn, mbstr);				// modbus murni
		hsl = cek_crc_mod(mbn, strmb3);
	//*/
	
	
	if (hsl==1 && mbn>=8)	{				// 8: min panjang modbus REQUEST
		//printf(" > LULUS < !!!\r\n");
		
		#if 0
		cmd = mbstr3[1];
		reg = (int) (mbstr[2]<<8 | mbstr[3]);
		jml = (int) (mbstr[4]<<8 | mbstr[5]);
		#endif
		
		cmd = strmb3[1];
		reg = (int) (strmb3[2]<<8 | strmb3[3]);
		jml = (int) (strmb3[4]<<8 | strmb3[5]);
		
		//printf("++++ cmd: 0x%02x, reg: 0x%02x, jml: %d\r\n", cmd, reg, jml);
		//cmd = parsing_mod(strSer2);
		if (cmd>0)	{
			//printf("__PROSES DATA KITA !!\r\n");
			//return respon_modbus(cmd, reg, jml, mbstr, mbn);
			return respon_modbus(cmd, reg, jml, strmb3, mbn);
		}
	}
	return 2;
}


void vAltStartCom3( unsigned portBASE_TYPE uxPriority, unsigned long ulBaudRate )		{
//const unsigned portBASE_TYPE uxQueueSize = 10;
//const unsigned portBASE_TYPE uxQueueLength = 256;
	//xPrintQueue2 = xQueueCreate( uxQueueSize, uxQueueLength );
	/* Initialise the com port then spawn the Rx and Tx tasks. */
	xSerialPortInit3( ulBaudRate, configMINIMAL_STACK_SIZE );
	qsprintf("masuk thread serial 3\r\n");
	/* The Tx task is spawned with a lower priority than the Rx task. */
	xTaskCreate( vComTask3, ( signed char * ) "Serial3", comSTACK_SIZE * ST_SER3, NULL, uxPriority, ( xTaskHandle * ) hdl_serial3 );
}

void init_banner3()		{
	printf("Task vComTask3 Init\r\n");
	//printd2(10, "masuk task RS485_2\r\n");
	//vSerialPutString2(xPort2, "vSerialPutString2\r\n", 6);
}

#endif
