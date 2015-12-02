
#ifndef __SH_RTOS__
#define __SH_RTOS__

#include "FreeRTOS.h"
#include "task.h"
//#include "sh_serial.h"
#include "tinysh/tinysh.h"

#define mainLIST_BUFFER_SIZE				512

void task_list();
//void task_run_time();
void cek_memory_stack();

#ifdef PAKAI_TINYSH
static tinysh_cmd_t task_list_cmd={ 0,"task_list","task list FreeRTOS","", task_list,0,0,0 };
static tinysh_cmd_t cek_stack_cmd={ 0,"cek_stack","cek stack memorty","", cek_memory_stack,0,0,0 };
//static tinysh_cmd_t task_run_time_cmd={ 0,"task_run_time","task run time FreeRTOS","", task_run_time,0,0,0 };
#endif

#endif
