#include "FreeRTOS.h"
#include "monita.h"
#include "ap_formdata.h"

int hitung_arith()
{
	int i,j,cnt_arith,no;
	
	cnt_arith = 0;
	
	struct t_data *st_data;		
	st_data = pvPortMalloc( PER_SUMBER * sizeof (struct t_data) );
	if (st_data == NULL)	{
		printf(" %s(): ERR allok memory gagal !\r\n", __FUNCTION__);
		vPortFree (st_data);
		return 69;
	}

	for (i=3; i<JML_SUMBER; i++)		
	{
		memcpy((char *) st_data, (char *) ALMT_DATA+(i*JML_KOPI_TEMP), (PER_SUMBER * sizeof (struct t_data)));
		for (j=0; j<PER_SUMBER; j++)
		{
			if (st_data[j].arith == 1)	
			{
				cnt_arith++;
				st_data[j].no_arith = (unsigned char) cnt_arith;
			}
			else st_data[j].no_arith = 0;
		}
	simpan_st_rom(SEKTOR_DATA, i, 1, (unsigned short *) st_data, 1);
	}

	vPortFree (st_data);	
	return cnt_arith;
}

