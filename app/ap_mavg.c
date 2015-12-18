
#include "FreeRTOS.h"
#include "monita.h"
#include "ap_mavg.h"

#ifdef MOV_AVG

extern volatile float data_f[];
extern struct t_mavg *st_mavg;

void reset_mavg(int max)
{
	int i;
	
	for (i=0; i<max; i++)
	{
		st_mavg[i].ke_4=0;
		st_mavg[i].ke_3=0;
		st_mavg[i].ke_2=0;
		st_mavg[i].ke_1=0;
		st_mavg[i].ke_0=0;
	}
		
}

void data_mavg(int max)
{
	int i;
	int no;
	
	for (i=0; i<max; i++)
	{
		no = st_mavg[i].nomer;
		//printf("|no=%d|\n\r",no);
		//st_mavg[i].ke_3=data_f[no];
		data_f[no] = (st_mavg[i].ke_0+st_mavg[i].ke_1+st_mavg[i].ke_2+st_mavg[i].ke_3+st_mavg[i].ke_4)/5;
		st_mavg[i].ke_4=st_mavg[i].ke_3;
		st_mavg[i].ke_3=st_mavg[i].ke_2;
		st_mavg[i].ke_2=st_mavg[i].ke_1;
		st_mavg[i].ke_1=st_mavg[i].ke_0;
	}
	//printf("|ma|\n\r");
	//printf("|<<<<<>>>>>|\n\r");
	
	#if 0
	struct t_data *st_data;
	
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
	#endif
}

int hitung_mavg()
{
	int i,j,cnt_mavg,no;
	
	cnt_mavg = 0;
	
	struct t_data *st_data;		
	st_data = pvPortMalloc( PER_SUMBER * sizeof (struct t_data) );
	if (st_data == NULL)	{
		printf(" %s(): ERR allok memory gagal !\r\n", __FUNCTION__);
		vPortFree (st_data);
		return 69;
	}

	for (i=0; i<JML_SUMBER; i++)		
	{
		memcpy((char *) st_data, (char *) ALMT_DATA+(i*JML_KOPI_TEMP), (PER_SUMBER * sizeof (struct t_data)));
		for (j=0; j<PER_SUMBER; j++)
		{
			if (st_data[j].mv_avg == 1)	
			{
				cnt_mavg++;
				st_data[j].no_ma = (unsigned char) cnt_mavg;
			}
			else st_data[j].no_ma = 0;
		}
	simpan_st_rom(SEKTOR_DATA, i, 1, (unsigned short *) st_data, 1);
	}

	vPortFree (st_data);	
	return cnt_mavg;
}

void ngurut_mavg()
{
	int i,j,cnt_mavg,no;
	struct t_data *st_data;
	
	cnt_mavg = 0;
	for (i=0; i<JML_SUMBER; i++)		
	{
		st_data = ALMT_DATA + i*JML_KOPI_TEMP;
		for (j=0; j<PER_SUMBER; j++)
		{
			if (st_data[j].mv_avg == 1)
			{
				no = i*PER_SUMBER+j;
				st_mavg[cnt_mavg].nomer = no;
				cnt_mavg ++;
			}
		}
	}
	
}
#endif
