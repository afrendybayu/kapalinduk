
// Afrendy Bayu
// BukitTanjung, 24 April 2015
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

enum xcmd_conf	{
	MB_REST, MB_REQ, MB_RESP, 
} cmd_mb_state		__attribute__ ((section (".usbram1")));

char* strcmd_srcmb[] 	= {" ","NATIVE", "PM810" , "SANTER" , "GWR" , NULL};
enum xcmd_conf_files	{
	NOL, SRC_MB_NATIVE, SRC_MB_PM810, SRC_MB_SANTER, SRC_MB_GWR
} cmd_conf_srcmb;


static xComPortHandle xPort3;//	__attribute__ ((section (".usbram1")));
//extern volatile struct t_st_hw st_hw;

//void sedot_mod(int ch);
//int proses_mod_cmd();

//extern char strmb[];

//int parsing_mod(unsigned char *x);
char strmb3[MAX_RX_MB3]		__attribute__ ((section (".usbram1")));
char outmb3[MAX_RX_MB3]		__attribute__ ((section (".usbram1")));


#if 0
static xQueueHandle xPrintQueue3;

// qsprintf : custom printf yg dapat menyimpan data ke queue
void qsprintf3(char *fmt, ...) {
	//uprintf("---> KIRIM : ");
	char str_buffer[256];
	int lg=0;
	va_list args;
	va_start(args, fmt);
	lg = vsprintf(str_buffer, fmt, args);
	lg = xQueueSend( xPrintQueue3, ( void * ) str_buffer, ( portTickType ) 0 );
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

int cmd_modbus(int gg, int *dReg, char src)	{
	struct t_sumber *st_sumber;
	st_sumber = (char *) ALMT_SUMBER;
	int destReg;
	int k;
	//char *ss;
	//if (gg==0)	printf("  ========================\r\n");
	//printf(" CMD Sumber %d\r\n",gg);
	//*
	if (st_sumber[gg].status)	{
		//printf("----> Smbr[%d]: %d : REQ MODBUS %s : ", gg, st_sumber[gg].status, st_sumber[gg].form);
		src = atoi(st_sumber[gg].form);
		if (src!=NOL)		{
			//parsing_mb_cmd(s, &cmd, &dest);
			parsing_mb_native_cmd(st_sumber[gg].form,outmb3,&destReg);
			#if 0
			printf(" CMD: ");
			for(k=0; k<8; k++)	{
				printf("%02X ", outmb3[k]);
			}
			printf("\r\n");
			#endif
			
			*dReg = destReg;
			//printf("Dest Reg: %d - %d\r\n", *dReg, destReg);
			return kirim_respon_mb(8,outmb3,50,3);
		}
	}
	//*/
	
	return 0;
}

int reset_giliran_mb(int g)	{
	
}

static portTASK_FUNCTION( vComTask3, pvParameters )		{
signed char cExpectedByte, cByteRxed;
portBASE_TYPE xResyncRequired = pdFALSE, xErrorOccurred = pdFALSE;
portBASE_TYPE xGotChar;
int ch;
char s[30];
char src;
	//char strmb[MAX_RX_MB];
	int  nmb = 0, balas = 0, dReg=0;
	char mb_state = MB_REST;
	char flag_ms = 0;
	short emergency_exit;
	/* Just to stop compiler warnings. */
	( void ) pvParameters;
	vTaskDelay(100);
	init_banner3();
	
	//nSer2 = 0;
	int mbgilir;	mbgilir=0;
	int loop2 = 0;
	//disTX3_485();
	enaTX3_485();
	enaRX3_485();
	
	st_hw.init++;
	
	do {
		vTaskDelay(100);
	} while(st_hw.init != uxTaskGetNumberOfTasks());
	vTaskDelay(10000);	

	for( ;; )	{
		if (mb_state==MB_REST)	{
			//printf("\r\n>>> MB_REST: %d   ", mbgilir);
			if (mbgilir==0)	vTaskDelay(500);
			else 			vTaskDelay(50);
			mb_state = MB_REQ;
		}
		else if (mb_state==MB_REQ)	{
			
			int rsp = cmd_modbus(mbgilir, &dReg, src); // << (ada bug) yang buat modbus master loop di RESP, sementara buat jalan keluar dlu. debug dsini takes long time
			//printf(">>> MB_REQ: %d dest: %d ", mbgilir, dReg);
			if (rsp>0)	
			{
				mb_state = MB_RESP;
				//printf(">>> MB_RESP: %d\r\n", mbgilir);
			}
			else 	{
				mb_state = MB_REST;
				mbgilir++;
				if (mbgilir>=JML_SUMBER)	mbgilir=0;
			}
		}
		else if (mb_state==MB_RESP)	{
			//printf(">>> MB_RESP: %d\r\n", mbgilir);
			xGotChar = xSerialGetChar3( xPort3, &ch, 100 );
			//printf("|%02x ", (char) ch);
			#if 1
			if( xGotChar == pdTRUE )		
			{
				if ((nmb==0) && ((char)ch==0xff))	
					{
					//printf("nmb: %d, ch: %02x, mask sini !!", nmb, (char)ch);
					//printf("kk %02x ", (char) ch);
					//printf("1.");
					} 
				else 
					{
					//printf("%02x ", (char) ch);
					//printf("%c ", (char) ch);
					//if (nmb>8)	{
						//nmb=0;
						strmb3[nmb] = (char) ch;
					//printf("x%02x ", strmb3[nmb]);
					//}
					nmb++;
					//printf("-%d ",nmb);
					//strSer2[nmb+1] = '\0';
					//sedot_mod(ch);
					//printf("2.");
					flag_ms=1;
					
					}
				//mb_state=MB_RESP;
			}
			else 
			{
				// sedot data respon (sendiri), clear buffer
				if ( (balas==nmb) && (balas>0) )	{			
					//printf("Reset MB2 !!!\r\n");
					nmb = 0;
					flag_ms = 0;
					//printf("3.");
					balas = 0;
				}
				
				if (flag_ms==1 && nmb>=8)	{
					//printf("hasil: %d\r\n", nmb);
					//printf("x%02x ", (char) ch);
					balas = proses_mod3(nmb, strmb3, dReg, src);					//printf("--==> BALAS MB: %d\r\n", balas);
					nmb = 0;
					//printf("4.");
					mb_state = MB_REST;
					emergency_exit = 0;
				}
				#if 0
				if (balas==0)	{
					nmb = 0;
				}
				//
				
				flag_ms = 0;
				nmb = 0;
				#endif
				
				mbgilir++;
				if (mbgilir>=JML_SUMBER)
				{
					mbgilir=0;
					emergency_exit ++;
				}
				
				// pintu darurat
				if (emergency_exit >= 10)
				{
					mb_state = MB_REST;
					emergency_exit = 0;
				}
				//printf("5.");
			
			}
			#endif
			//*/

			
			//
		}
		
		
	}
}

int proses_mod3(int mbn, char *mbstr, int dReg, char src)	{
	int hsl=0, cmd=0, jml=0, reg=0;
	int i,mm=(mbn-8);
	char *ss;
	char *ss2;
	unsigned int det;
	
	if (mbn<=8)	return 0;
	ss = &mbstr[8];
	//ss2 = &mbstr[0];
	
	//det = (unsigned int) ss[2] / (unsigned int) ss2[5];
	//printf("[%d]",det);
	
	#if 0
	printf("\r\nJml Respon: %d -->", mbn);
	for (i=0; i<(mbn-8); i++)		{
		//printf(" %02x", mbstr[i]);
		printf(" %02x", ss[i]);
	}
	printf("\r\n");
	#endif
	
	#ifdef ERROR_DATA_RATE
		if (dari == 0xFFFF) dari = 0;
		dari++;
		printf("received=%d\n\r",dari);
	#endif
	
	// CEK CRC
	hsl = get_crc_mod(mm-2, ss);	
	//printf("crc : %04x\r\n", hsl);
	if (((hsl>>8 & 0xFF) != ss[mm-1]) || ((hsl & 0xFF) != ss[mm-2]))	return 1;
	
	#if 1
	if (ss[1]==READ_HOLDING_REG){
	switch (src){
		case SRC_MB_SANTER:
			simpan_mb_monita(ss[2], &ss[3], dReg); break;
		case SRC_MB_GWR:
			simpan_mb_gwr(ss[2], &ss[3], dReg); break;
		case SRC_MB_NATIVE:
			simpan_mb_std(ss[2], &ss[3], dReg); break;
		case SRC_MB_PM810:
			printf("810\n\r"); break;
		default :
			printf("invalid command\n\r"); return 0;
	}
	}
	else{
		printf("unavailable command\n\r");
		return 0;
	}
	#endif
	
	#if 0
	//printf("lanjut ....dReg: %d\r\n", dReg);
	if ((ss[1]==READ_HOLDING_REG) && det==4) simpan_mb_monita(ss[2], &ss[3], dReg);
	else simpan_mb_std(ss[2], &ss[3], dReg);
	//if (ss[1]==READ_HOLDING_REG)	simpan_nilai_mb(ss[2], &ss[3], dReg);
	#endif
	
	return (mbn-8);
	
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
	//printd3(10, "masuk task RS485_2\r\n");
	//vSerialPutString3(xPort3, "vSerialPutString2\r\n", 6);
}

#endif
