
#include "FreeRTOS.h"
#include "task.h"

#ifdef PAKAI_TINYSH
#include "tinysh/tinysh.h"
#endif

//#include "sh_serial.h"
#include "sh_rtos.h"

extern xTaskHandle *hdl_shell;
extern xTaskHandle *hdl_led;

void task_list()	{
#if INCLUDE_vTaskSuspend==0
	qsprintf("\r\n--> task list NON AKTIF, INCLUDE_vTaskSuspend perlu diaktifkan.\r\n");
#else
static signed char cListBuffer[ mainLIST_BUFFER_SIZE ];
const signed char *pcList = &( cListBuffer[ 0 ] );
const char * const pcHeader = "Task          State  Priority  Stack	#\r\n************************************************\r\n";
	sprintf(cListBuffer, "\r\nJml Task: %d\r\n", uxTaskGetNumberOfTasks());
	qsprintf(cListBuffer);
	qsprintf(pcHeader);
	vTaskList( cListBuffer );
	qsprintf(cListBuffer);
	printf("\r\n");
#endif
}

void cek_memory_stack()		{
#if 0
	printf("\r\n");
	printf("  Jml min stack space task shell : %d\r\n", uxTaskGetStackHighWaterMark( hdl_shell ));
	printf("  Jml min stack space task LED   : %d\r\n", uxTaskGetStackHighWaterMark( hdl_led ));
	
	#ifdef PAKAI_SERIAL_2
	//printf("  Jml min stack space task LED   : %d\r\n", uxTaskGetStackHighWaterMark( &hdl_led );
	#endif
#endif
}


#if 0
void task_run_time()	{
static signed char cListBuffer[ mainLIST_BUFFER_SIZE ];
const signed char *pcList = &( cListBuffer[ 0 ] );
const char * const pcHeader = "\r\nTask          Time  %	#\r\n************************************************\r\n";
	qsprintf(pcHeader);
	vTaskGetRunTimeStats( cListBuffer );
	qsprintf(cListBuffer);
}
#endif
