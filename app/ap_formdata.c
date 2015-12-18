#include "FreeRTOS.h"
#include "monita.h"
#include "ap_formdata.h"

int hitung_arith()
{
	int i,j,cnt_arith,no;
	
	cnt_arith = 0;
	
	struct t_data *st_data;		

	for (i=3; i<JML_SUMBER; i++)		
	{
		st_data = ALMT_DATA + i*JML_KOPI_TEMP;
		for (j=0; j<PER_SUMBER; j++)
		{
			if (st_data[j].arith)	
			{
				cnt_arith++;
			}
		}
	}

	return cnt_arith;
}

