
#include "FreeRTOS.h"
#include "monita.h"
#include "ap_mavg.h"

#ifdef MOV_AVG

extern volatile float data_f[];

void data_mavg(int cnt)
{
	int i;

	struct t_data *st_data;

	for (i=0; i<JML_SUMBER; i++)		
	{
		st_data = ALMT_DATA + i*JML_KOPI_TEMP;
		for (j=0; j<PER_SUMBER; j++)	{
			if (st_data[i].mv_avg == 1)	
			{
				data_f[i] = (mavg.ke_0[i]+mavg.ke_1[i]+mavg.ke_2[i]+mavg.ke_3[i]+mavg.ke_4[i])/5;
				mavg.ke_4[i]=mavg.ke_3[i];
				mavg.ke_3[i]=mavg.ke_2[i];
				mavg.ke_2[i]=mavg.ke_1[i];
				mavg.ke_1[i]=mavg.ke_0[i];
			}
		}	
	}
}

int hitung_mavg()
{
	int i,cnt_mavg;
	
	cnt_mavg = 0;
	struct t_data *st_data;
	st_data = ALMT_DATA;	

	for (i=0; i<JML_TITIK_DATA; i++)		
	{
		if (st_data[i].mv_avg == 1)	
		{
		cnt_mavg++;
		}
	}
	
	return cnt_mavg;
}
#endif
