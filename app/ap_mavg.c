
#include "FreeRTOS.h"
#include "monita.h"
#include "ap_mavg.h"

#ifdef MOV_AVG

extern volatile float data_f[];

void data_mavg(int cnt)
{
	int i,j;
	int no;

	struct t_data *st_data;
	
	struct t_mavg *st_mavg;
	st_mavg = pvPortMalloc( cnt * sizeof (struct t_mavg) );
	if (st_mavg == NULL)	{
		printf(" %s(): ERR allok memory gagal !\r\n", __FUNCTION__);
		vPortFree (st_mavg);
	}
	
	for (i=0; i<JML_SUMBER; i++)		
	{
		st_data = ALMT_DATA + i*JML_KOPI_TEMP;
		for (j=0; j<PER_SUMBER; j++)	
		{
			no = i*PER_SUMBER+j;
			if (st_data[j].mv_avg == 1)	
			{
				data_f[no] = (st_mavg[no].ke_0+st_mavg[no].ke_1+st_mavg[no].ke_2+st_mavg[no].ke_3+st_mavg[no].ke_4)/5;
				st_mavg[no].ke_4=st_mavg[no].ke_3;
				st_mavg[no].ke_3=st_mavg[no].ke_2;
				st_mavg[no].ke_2=st_mavg[no].ke_1;
				st_mavg[no].ke_1=st_mavg[no].ke_0;
			}
		}	
	}
	vPortFree(st_mavg);
}

int hitung_mavg()
{
	int i,j,cnt_mavg;
	
	cnt_mavg = 0;
	struct t_data *st_data;

	for (i=0; i<JML_SUMBER; i++)		
	{
		st_data = ALMT_DATA + i*JML_KOPI_TEMP;
		for (j=0; j<PER_SUMBER; j++)
		{
			if (st_data[j].mv_avg == 1)	cnt_mavg++;
		}
	}
	
	return cnt_mavg;
}
#endif
