
#include "FreeRTOS.h"
#include "task.h"

#include "sh_tarik.h"
#include "monita.h"
#include "sh_utils.h"
#include "manual.h"


extern volatile float data_f[];


void tarik_data()
{
	int i,j;
	struct t_data *st_data;
	
	uprintf("\n\r");
	for (i=0; i < JML_RINJANI; i++)
	{
		st_data = ALMT_DATA + i*JML_KOPI_TEMP;
		for (j=0; j < (PER_SUMBER-(6*i)); j++)
		{
			uprintf("%d:%.2f\n\r", st_data[j].id, data_f[i*PER_SUMBER+j]);
		}
	}
}
