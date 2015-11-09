
// Afrendy Bayu
// Depok, 25 Maret 2013
// 

/* Scheduler include files. */

//#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include <stdint.h>

/* Demo program include files. */
#include "serial/serial.h"

#ifdef PAKAI_TINYSH
#include "tinysh/tinysh.h"
#endif

#include "ap_serial.h"
#include <stdarg.h>
#include "queue.h"
#include "semphr.h"

#include "hardware.h"
#include "monita.h"
#include "iap.h"

#include "sh_hardware.h"
#include "sh_sumber.h"
#include "sh_env.h"
#include "sh_rtos.h"
#include "sh_data.h"
#include "sh_kanal.h"

#include "sh_utils.h"

#ifdef PAKAI_FREERTOS_CLI
	#include "FreeRTOS-Plus-CLI/FreeRTOS_CLI.h"
#endif

#ifdef PAKAI_ADC_7708
	#include "sh_adc.h"
#endif

#ifdef PAKAI_RTC
	#include "sh_rtc.h"
#endif

#ifdef PAKAI_SDCARD
//	#include "sh_sdc.h"
	#include "ff/fatfs/shell_fs.h"
	#include "ap_file.h"
	#include "sh_file.h"
#endif

static xComPortHandle xPort;

#ifdef PAKAI_SHELL
extern xSemaphoreHandle xSemSer0;
static xQueueHandle xPrintQueue;
xTaskHandle *hdl_shell;
extern volatile struct t_st_hw st_hw;

#if 1

void printd(int prio, const char *format, ...)	{
	va_list arg;
	int lg=0;
	
	struct t_env *st_env;
	st_env = ALMT_ENV;
	
	char str_buffer[128];
	//if (prio>0)	{
	if (prio>=st_env->prioDebug)	{
		if( xSemSer0 != NULL )    {
			if( xSemaphoreTake( xSemSer0, ( portTickType ) 10 ) == pdTRUE )	{
				va_start (arg, format);
				lg = vsprintf (str_buffer, format, arg);	
				va_end (arg);
				vSerialPutString(xPort, format, lg);
				xSemaphoreGive( xSemSer0 );
			}
		}
		vTaskDelay(5);
	}
}


// qrprintf : custom printf yg dapat mengambil data dari queue
void qrprintf(portTickType w) {
	//char *str_buffer;
	if( xSemSer0 != NULL )    {
        // See if we can obtain the semaphore.  If the semaphore is not available
        // wait 10 ticks to see if it becomes free.	
        if( xSemaphoreTake( xSemSer0, ( portTickType ) 10 ) == pdTRUE )	{
			char str_buffer[128];
			if (xPrintQueue != 0)	{
				if (xQueueReceive( xPrintQueue, &str_buffer, w ))	{
					//uprintf("PrintQueue: %d/%d__%s\r\n", xPrintQueue, strlen(str_buffer), str_buffer);
					//uprintf("%d__%s\r\n", strlen(str_buffer), str_buffer);
					vSerialPutString(xPort, str_buffer, strlen(str_buffer));
				}
			}
			xSemaphoreGive( xSemSer0 );
		}
	}
	//vTaskDelay(5);
}


#endif

void vAltStartComTestTasks( unsigned portBASE_TYPE uxPriority, unsigned long ulBaudRate )		{
const unsigned portBASE_TYPE uxQueueSize = 10;
const unsigned portBASE_TYPE uxQueueLength = 128;
	xPrintQueue = xQueueCreate( uxQueueSize, uxQueueLength );
	/* Initialise the com port then spawn the Rx and Tx tasks. */
	xSerialPortInitMinimal( ulBaudRate, configMINIMAL_STACK_SIZE );

	/* The Tx task is spawned with a lower priority than the Rx task. */
	// uxPriority = uxPriority
	xTaskCreate( vComRxTask, ( signed char * ) "Shell", comSTACK_SIZE * ST_SHELL, NULL, uxPriority, ( xTaskHandle * ) hdl_shell );
}

void init_banner()	{
	IAP_return_t iap_return;
	iap_return = iapReadSerialNumber();
//const unsigned portBASE_TYPE uxQueueSize = 10;
//const unsigned portBASE_TYPE uxQueueLength = 30;
	//xPrintQueue = xQueueCreate( uxQueueSize, ( unsigned portBASE_TYPE ) sizeof( char * ) );
	//xPrintQueue = xQueueCreate( uxQueueSize, uxQueueLength );
	//strcpy(s, "percobaan queue init_banner ...\r\n");
	//xQueueSend( xPrintQueue, ( void * ) s, ( portTickType ) 10 );
	//uprintf("xPrintQueue hasil queue: %d\r\n", xPrintQueue);
	uprintf("\r\n\r\nDaun Biru Engineering, Maret 2013\r\n");
	uprintf("==================================\r\n");
	uprintf("monita %s %s\r\n", BOARD_SANTER, BOARD_SANTER_versi);
	uprintf("CPU %s, %d MHz", uC, configCPU_CLOCK_HZ/1000000);
	if (iap_return.ReturnCode == 0)
		uprintf(", P/N 0x%08X", iap_return.Result[0]);
	iap_return = iapReadBootVersion();
	if (iap_return.ReturnCode == 0)
		uprintf(", BootV0x%04X\r\n", (unsigned short) iap_return.Result[0]);
	else {
		uprintf("-------\r\n");
	}
	uprintf("FreeRTOS %s by Richard Barry\r\n", tskKERNEL_VERSION_NUMBER);		//  oleh Richard Barry
	uprintf("ARM-GCC %s : %s : %s\r\n", __VERSION__, __DATE__, __TIME__);
	
}

#if PAKAI_CONTOH_SHELL
static void display_args(int argc, char **argv)	{
	int i;
	for(i=0;i<argc;i++)
		uprintf("  argv[%d]=\"%s\"\r\n",i,argv[i]);
}

static void foo_fnt(int argc, char **argv)	{
	printf("foo command called\r\n");
	display_args(argc,argv);
}

static tinysh_cmd_t myfoocmd={0,"foo","foo command","[args]", foo_fnt,0,0,0};


static void atoxi_fnt(int argc, char **argv)	{
	int i;
	for(i=1;i<argc;i++)    {
		uprintf("\"%s\"-->%u (0x%x)\r\n",
		argv[i],tinysh_atoxi(argv[i]),tinysh_atoxi(argv[i]));
	}
}
#endif

void cmd_shell()	{
#ifdef PAKAI_TINYSH
	#ifdef PAKAI_CONTOH_SHELL
	tinysh_add_command(&myfoocmd);
	#endif
	
	tinysh_add_command(&cek_env_cmd);
	tinysh_add_command(&set_env_cmd);
	tinysh_add_command(&cek_struct_cmd);

	tinysh_add_command(&set_kanal_cmd);
	tinysh_add_command(&cek_kanal_cmd);
	
	tinysh_add_command(&set_data_cmd);
	tinysh_add_command(&cek_data_cmd);
	
	tinysh_add_command(&cek_sumber_cmd);
	tinysh_add_command(&set_sumber_cmd);
//	tinysh_add_command(&cek_sumber_temp_cmd);
	
	tinysh_add_command(&reset_cmd);
	tinysh_add_command(&task_list_cmd);
//	tinysh_add_command(&cek_stack_cmd);
//	tinysh_add_command(&task_run_time_cmd);
	tinysh_add_command(&idle_tick_cmd);
	
	
	tinysh_add_command(&sektor_free_cmd);
	tinysh_add_command(&hapus_sektor_rom_cmd);
	tinysh_add_command(&simpan_sektor_rom_cmd);
	tinysh_add_command(&simpan_struct_rom_cmd);
	tinysh_add_command(&kopi_sektor_cmd);
	
	tinysh_add_command(&baca_rom_cmd);

	
	#ifdef PAKAI_ADC_7708
	tinysh_add_command(&cek_adc_cmd);
	#endif
	
	#ifdef PAKAI_RTC
	tinysh_add_command(&set_date_cmd);
	//tinysh_add_command(&cek_flag_rtc_cmd);
	//tinysh_add_command(&init_rtc_cmd);
	tinysh_add_command(&kalender_rtc_cmd);
	tinysh_add_command(&uptime_cmd);
	tinysh_add_command(&set_rtc_mem_cmd);
	tinysh_add_command(&cek_rtc_mem_cmd);
	#endif

#if 0
	#ifdef PAKAI_SDCARD
	tinysh_add_command(&cek_sdc_cmd);
	tinysh_add_command(&cek_free_cluster_cmd);
	tinysh_add_command(&cek_read_cmd);
	//tinysh_add_command(&cek_read_sektor_cmd);
	tinysh_add_command(&cek_pwd_cmd);
	tinysh_add_command(&cek_ls_cmd);
	#endif
#endif

	#ifdef PAKAI_SDCARD
	tinysh_add_command(&util_cd_cmd);
	tinysh_add_command(&util_pwd_cmd);
	tinysh_add_command(&util_ls_cmd);
	tinysh_add_command(&util_mkdir_cmd);
	tinysh_add_command(&util_view_cmd);
	tinysh_add_command(&util_rm_cmd);
	tinysh_add_command(&simpan_file_cmd);
	
	tinysh_add_command(&cek_file_cmd);
	tinysh_add_command(&set_file_cmd);
	tinysh_add_command(&upload_file_cmd);
	#endif
	
	/*
	#ifdef PAKAI_SERIAL_2
	tinysh_add_command(&cek_serial2_cmd);
	#endif
	
	#ifdef PAKAI_SERIAL_3
	tinysh_add_command(&cek_serial3_cmd);
	#endif
 	//*/

	
#endif
}

#ifdef PAKAI_FREERTOS_CLI
static portBASE_TYPE printCommand( int8_t *pcWriteBuffer,
                                          size_t xWriteBufferLen,
                                          const int8_t *pcCommandString )
{
	( void ) xWriteBufferLen;
	uprintf("CMD: %s\r\n", __FUNCTION__);
	return pdFALSE;
}

static portBASE_TYPE prvTaskStatsCommand( int8_t *pcWriteBuffer,
                                          size_t xWriteBufferLen,
                                          const int8_t *pcCommandString )
{
    /* For simplicity, this function assumes the output buffer is large enough
    to hold all the text generated by executing the vTaskList() API function,
    so the xWriteBufferLen parameter is not used. */
    ( void ) xWriteBufferLen;

    /* pcWriteBuffer is used directly as the vTaskList() parameter, so the table
    generated by executing vTaskList() is written directly into the output
    buffer. */
    //vTaskList( pcWriteBuffer + strlen( pcHeader ) );
    vTaskList( pcWriteBuffer );

    /* The entire table was written directly to the output buffer.  Execution
    of this command is complete, so return pdFALSE. */
    return pdFALSE;
}


static portBASE_TYPE prvRunTimeStatsCommand( int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString )
{
const int8_t * const pcHeader = ( int8_t * ) "Task            Abs Time      % Time\r\n****************************************\r\n";

	/* Remove compile time warnings about unused parameters, and check the
	write buffer is not NULL.  NOTE - for simplicity, this example assumes the
	write buffer length is adequate, so does not check for buffer overflows. */
	( void ) pcCommandString;
	( void ) xWriteBufferLen;
	configASSERT( pcWriteBuffer );

	/* Generate a table of task stats. */
	strcpy( ( char * ) pcWriteBuffer, ( char * ) pcHeader );
	vTaskGetRunTimeStats( pcWriteBuffer + strlen( ( char * ) pcHeader ) );

	/* There is no more data to return after this single string, so return
	pdFALSE. */
	return pdFALSE;
}



static const CLI_Command_Definition_t xTaskStats =
{
	( const int8_t * const ) "printx", /* The command string to type. */
	( const int8_t * const ) "\r\ntask-stats:\r\n Displays a table showing the state of each FreeRTOS task\r\n\r\n",
	printCommand, /* The function to run. */
	0 /* No parameters are expected. */
};

void vRegisterCLICommands( void )		{
	/* Register all the command line commands defined immediately above. */
	FreeRTOS_CLIRegisterCommand( &xTaskStats );
}
#endif

static portTASK_FUNCTION( vComRxTask, pvParameters )		{
signed char cExpectedByte, cByteRxed;
portBASE_TYPE xResyncRequired = pdFALSE, xErrorOccurred = pdFALSE;
portBASE_TYPE xGotChar;
int ch, mm=0;
char s[30];
int jml_mavg;
extern struct t_mavg *st_mavg;

	/* Just to stop compiler warnings. */
	( void ) pvParameters;
	vTaskDelay(1);

	//vSerialPutString(xPort, "mulakan\r\n", 9);
	init_banner();
	//set_env_default();
	baca_konfig_rom();					// hardware/iap.c
	//load_data_rtc();
	
	#ifdef MOV_AVG
	struct t_env *st_env;
	st_env = pvPortMalloc( sizeof (struct t_env) );
	if (st_env == NULL)	{
		uprintf(" %s(): ERR allok memory gagal !\r\n", __FUNCTION__);
		vPortFree(st_env);
	}

	memcpy((char *) st_env, (char *) ALMT_ENV, (sizeof (struct t_env)));	
	jml_mavg = hitung_mavg();
	st_env->n_mavg = jml_mavg;
	simpan_st_rom(SEKTOR_ENV, ENV, 0, (unsigned short *) st_env, 0);
	vPortFree (st_env);
	
	st_mavg = pvPortMalloc( jml_mavg * sizeof (struct t_mavg) );
	if (st_mavg == NULL)	{
		uprintf(" %s(): ERR allok memory gagal !\r\n", __FUNCTION__);
		vPortFree (st_mavg);
	}
	ngurut_mavg();
	reset_mavg(jml_mavg);
	#endif	
	
	cmd_shell();
	st_hw.init++;
	
	#ifdef PAKAI_RTC
		//init_RTC_sh();
		start_uptime();
	#endif
	
	#ifdef PAKAI_SDCARD
		st_hw.sdc = 0;
		//disk_initialize(SDC);
		disk_initialize(0);
		set_fs_mount();
		cek_fs_free();
		//mount_disk(0);		// 0: SDCARD
		//uprintf("Cek Memori SDCARD: ...");
		//cek_free_cluster();
		st_hw.sdc = 1;
	#endif
	
	#ifdef configUSE_IDLE_HOOK
		st_hw.init++;
	#endif

	do	{
		vTaskDelay(100);
	} while (st_hw.init != uxTaskGetNumberOfTasks());
	
	//vTaskDelay(100);
	#ifdef PAKAI_SDCARD
	//uprintf("Cek Memori SDCARD: ...");
	//cek_free_cluster();
	#endif
	vTaskDelay(100);
	sprintf(s, "%s$ ", PROMPT);
	
	
	#ifdef PAKAI_FREERTOS_CLI		// gak jadi pake FreeRTOS-CLI
	uprintf(s);
	vRegisterCLICommands();
	
	for(;;)	{
		//uprintf("Merdeka!!!\r\n");
		//vSerialPutString(xPort, "tes\r\n", 5);
		xGotChar = xSerialGetChar( xPort, &ch, 10 );
		if( xGotChar == pdTRUE )		{
			//if( xSerialGetChar( xPort, &ch, comRX_BLOCK_TIME ) )		{ // comRX_BLOCK_TIME = 0xffff
			//tinysh_char_in((unsigned char) ch);
			toogle_led_utama();
			if ((uchr) ch=='\r')	{
				sprintf(s, "\r\n%s$ ", PROMPT);
				uprintf(s);
			}
		}
		vTaskDelay(10);
	}
	#endif
	
	#ifdef PAKAI_TINYSH
	tinysh_set_prompt(s);
	tinysh_char_in('\r');
	vTaskDelay(500);
	for( ;; )	{
		//vTaskDelay(10);
		//printf("testing\r\n");
		xGotChar = xSerialGetChar( xPort, &ch, 10 );
		
		if( xGotChar == pdTRUE )		{
			tinysh_char_in((unsigned char) ch);
			toogle_led_utama();
		}
		if (st_hw.mm>=120)	{			// cron tiap 1 menit
		//if (st_hw.mm >= 10)	{			// cron tiap 10detik
		//if (st_hw.mm>=2)		{			// cron tiap 1 detik
			st_hw.mm = 0;
			st_hw.uuwaktu++;
			#ifdef PAKAI_FILE_SIMPAN
			simpan_file_data();
			#endif
		}
		
		qrprintf(0);
	}
	#endif	
	
	for( ;; )	{
		vTaskDelay(10);
	}
}
#endif

// qsprintf : custom printf yg dapat menyimpan data ke queue
void qsprintf(char *fmt, ...) {
#ifdef PAKAI_SHELL
	//uprintf("---> KIRIM : ");
	char str_buffer[128];
	int lg=0;
    va_list args;
    va_start(args, fmt);
    lg = vsprintf(str_buffer, fmt, args);
    lg = xQueueSend( xPrintQueue, ( void * ) str_buffer, ( portTickType ) 0 );
    va_end(args);
    //uprintf(" %d/%d >>>>> %s\r\n", lg, xPrintQueue, str_buffer);
#endif
}

// uprintf : user/custom printf ke serial debug
char str_buffer[MAX_DEBUG_TX]		__attribute__ ((section (".usbram1")));
void uprintf(char *fmt, ...) {
#ifdef PAKAI_SHELL
//#if 1
	
	int lg=0;
    va_list args;
    va_start(args, fmt);
    lg = vsprintf(str_buffer, fmt, args);
    va_end(args);
    if( xSemSer0 != NULL )    {
		if( xSemaphoreTake( xSemSer0, ( portTickType ) 10 ) == pdTRUE )	{
			vSerialPutString(xPort, str_buffer, 0xffff);
			xSemaphoreGive( xSemSer0 );
		}
	}
	vTaskDelay(10);
#endif
}

int printf0 (const char *fmt, ...)		{
	va_list args;
	int i;
	//char printbuffer[256];
	va_start (args, fmt);

	i = vsprintf (str_buffer, fmt, args);
	va_end (args);

	/* Print the string */
	if( xSemSer0 != NULL )    {
		if( xSemaphoreTake( xSemSer0, ( portTickType ) 10 ) == pdTRUE )	{
			vSerialPutString(xPort, str_buffer, 0xffff);
			xSemaphoreGive( xSemSer0 );
		}
	}
	vTaskDelay(1);
	return 0;
} 
