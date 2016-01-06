
// Afrendy Bayu
// Depok, 25 Maret 2013
// 

/* Scheduler include files. */


#include "FreeRTOS.h"
#include "task.h"

/* Demo program include files. */
#include "serial/serial.h"
#include "sh_modem.h"
//#include "modbus/mb.h"
#include "ap_serial1.h"
//#include "sh_serial.h"
//#include <stdarg.h>
#include "queue.h"
#include "hardware.h"
#include "monita.h"
#include <stdarg.h>
//#include "sh_hardware.h"

#ifdef PAKAI_SERIAL_1

char sCmdM[200];
char sRespM[200];


#define CMD_LIST_SMS	"AT+CMGL=\"ALL\"\r\n"
#define CMD_DEL_SMS		"AT+CMGD="
#define CMD_CEK_PULSA	"AT+CUSD=1,"
#define CMD_BACA_SMS	"AT+CMGR="
#define CMD_SMS_ASCII	"AT+CMGF=1\r\n"
#define CMD_SMS_PDU		"AT+CMGF=0\r\n"
#define CMD_KIRIM_SMS	"AT+CMGS="
#define CMD_RESET_MODEM	"AT+CFUN=1\r\n"
#define CMD_SET_MODEM	"AT+CFUN=0\r\n"
#define CMD_SIG_MODEM	"AT+CSQ\r\n"			// 11-31: mantap


#define CTRL_Z			26
#define HP_DEFAULT		"+6282114722505"

#define PULSA_SIMPATI	"*888#"
#define PULSA_AXIS		"*888#"

#define MAX_LOOP_MODEM	5
char * smsDibalas[] = {"pulsa", "data", "nama", "satuan"};

static xComPortHandle xPort1;
//extern volatile struct t_st_hw st_hw;

//void sedot_mod(int ch);
//int proses_mod_cmd();

//int parsing_mod(unsigned char *x);
//char strmb1[MAX_RX_MB]		__attribute__ ((section (".usbram2")));
//char outmb1[MAX_RX_MB]		__attribute__ ((section (".usbram2")));

#if 0
static xQueueHandle xPrintQueue1;

// qsprintf : custom printf yg dapat menyimpan data ke queue
void qsprintf1(char *fmt, ...) {
	//uprintf("---> KIRIM : ");
	char str_buffer[256];
	int lg=0;
	va_list args;
	va_start(args, fmt);
	lg = vsprintf(str_buffer, fmt, args);
	lg = xQueueSend( xPrintQueue1, ( void * ) str_buffer, ( portTickType ) 0 );
	va_end(args);

}
#endif


void printd1(int prio, const char *format, ...)	{
	va_list arg;
	int lg=0;
	char str_buffer[256];
	if (prio>0)	{
	//if (prio>env.prioDebug2)	{
		; (arg, format);
		lg = vsprintf (str_buffer, format, arg);	
		va_end (arg);
		//printf("%s", format);
		//enaTX2_485();
		vSerialPutString1(xPort1, format, lg);
		vTaskDelay(10);
		//disTX2_485();
	}
}




static portTASK_FUNCTION( vComTask1, pvParameters )		{
signed char cExpectedByte, cByteRxed;
portBASE_TYPE xResyncRequired = pdFALSE, xErrorOccurred = pdFALSE;
portBASE_TYPE xGotChar;
int ch;
char l;
char s[30],s1;


	//char strmb[MAX_RX_MB];
	int  nmb = 0, balas = 0;
	char flag_ms = 0;
	/* Just to stop compiler warnings. */
	( void ) pvParameters;
	vTaskDelay(100);
	//init_banner2();
	
	vTaskDelay(500);
	st_hw.init++;
	//nSer2 = 0;
	int loop1 = 0;
	//disTX2_485();
	//enaRX2_485();
	
	do {
		vTaskDelay(100);
	} while(st_hw.init != uxTaskGetNumberOfTasks());

	
		
	for( ;; )	{
		vTaskDelay(10);
		
	//	xGotChar = xSerialGetChar1( xPort1, &s1, 10);
		
		
		
		
	}

}

void perintah_modem(char cmd)	{
		extern char index_gsm;
		extern char status_modem;

		if (status_modem)	{
			printf("Modem SIBUK !!! Tunggu dulu\r\n");
			// masukkan antrian job modem !!
			vTaskDelay(2000);
			return 0;
		}
		
		char hasil_modem=0;		
		status_modem = 1;
		//bersih_bersih();
		if (cmd==CEK_AT)
			hasil_modem = cek_AT_cmd();
		if (cmd==ECHO_OFF)
			hasil_modem = Echo_off_cmd();
		if (hasil_modem)
			flagModem = 0;
		
		status_modem = 0;
		//printf("selesai perintah modem !!!\r\n");
	}

int cek_AT_cmd()	{
		kirimCmdModem("AT\r\n",sRespM);		
		if (strncmp(sRespM,"OK", 2)!=0)	{
		//if (strncmp(sRespM,"\r\n", 2)!=0)
				printf("Respon : %s\r\n", sRespM);	}
			
			kirimCmdModem("AT+CGATT=1\r\n",sRespM);	
		if (strncmp(sRespM,"OK", 2)!=0)	{
		//if (strncmp(sRespM,"\r\n", 2)!=0)
				printf("Respon : %s\r\n", sRespM);	}
		
		kirimCmdModem("AT+CGDCONT=1,\"IP\",\"internet\"\r\n",sRespM);		
		if (strncmp(sRespM,"OK", 2)!=0)	{
		//if (strncmp(sRespM,"\r\n", 2)!=0)
				printf("Respon : %s\r\n", sRespM);	}
		kirimCmdModem("AT+CGACT=1,1\r\n",sRespM);		
		if (strncmp(sRespM,"OK", 2)!=0)	{
		//if (strncmp(sRespM,"\r\n", 2)!=0)
				printf("Respon : %s\r\n", sRespM);	}
		kirimCmdModem("AT+CGPADDR=1\r\n",sRespM);		
		if (strncmp(sRespM,"OK", 2)!=0)	{
		//if (strncmp(sRespM,"\r\n", 2)!=0)
				printf("Respon : %s\r\n", sRespM);	}
		
		
		
		return 1;
	}


int Echo_off_cmd() {
		kirimCmdModem("ATE1\r\n",sRespM);
		if (strncmp(sRespM,"OK", 2)!=0)	{
		//if (strncmp(sRespM,"\r\n", 2)!=0)
				printf("Respon : %s\r\n", sRespM);}
		kirimCmdModem("AT&W\r\n",sRespM);
		if (strncmp(sRespM,"OK", 2)!=0)	{
		//if (strncmp(sRespM,"\r\n", 2)!=0)
				printf("Respon : %s\r\n", sRespM);}
	return 1;
}


int kirimCmdModem(char *psCmdM,char *psRespM)	{
		int mm;
		char cM[2];
		int nn = 0, mx=0;
		
	
		
		//printf("===> kirim : %s", psCmdM);
		//if (flagModem)	{
		//	flagModem = 0;		// Tanda modem SIBUK !!!
		//vTaskDelay(100);
			vSerialPutString1(xPort1,psCmdM,10)           ;
		//vTaskDelay(10);	
				for (mm=0; mm<nRespM; mm++)	{	// sebanyak jml karakter SMS
						vTaskDelay(10);
				if (xSerialGetChar1(xPort1, &cM[0], 10) == pdTRUE)		{
						
						//printf("%s",sRespM);
						
						if (((cM[1]=='O') && (cM[0]=='K')))	{
							//printf("-----0x0A---------------\r\n");
							nn++;
							sRespM[mx] = cM[0];
						break;   
						} 
						else if (cM[0]==0x0D)	{
							continue;
						} else {
							nn++;
							sRespM[mx] = cM[0];
						}
						cM[1]=cM[0];
						mx++;
				}
					
					//printf(" +++ mm: %d\r\n", mm);
					
				}
				psRespM[nn] = '\0';
				//printf(" +++++++ Respon : %s\r\n", sRespM);
				
			return nn;
		//}
		//flagModem = 1;			// Tanda modem FREE !!!
	}
	
	
void vAltStartCom1( unsigned portBASE_TYPE uxPriority, unsigned long ulBaudRate )		{
//const unsigned portBASE_TYPE uxQueueSize = 10;
//const unsigned portBASE_TYPE uxQueueLength = 256;
	//xPrintQueue2 = xQueueCreate( uxQueueSize, uxQueueLength );
	/* Initialise the com port then spawn the Rx and Tx tasks. */
	xSerialPortInit1( ulBaudRate, configMINIMAL_STACK_SIZE );

	/* The Tx task is spawned with a lower priority than the Rx task. */
	xTaskCreate( vComTask1, ( signed char * ) "Serial1", comSTACK_SIZE * ST_SER1, NULL, uxPriority, ( xTaskHandle * ) hdl_serial1 );
}


void init_banner1()		{
	printf("Task vComTask2 Init\r\n");
	
}

#endif
