

// Jefri Sihombing
// Depok, 13 April 2015
// 


#include "FreeRTOS.h"
#include "task.h"
#include "monita.h"
#include "ap_astm.h"
#include "math.h"

struct t_astm *st_astm;

int lok_suhu(float cuhu)
{
	int poss,q;
	float cuhu_temp;
	
	//pless_temp = pless_temp+0.5;
	//q = (int) pless_temp;
	
	cuhu_temp = cuhu-25;
	//cuhu_temp = cuhu_temp*4 +1;
	cuhu_temp = cuhu_temp*4;
	q = (int) cuhu_temp;
	//w = w+1;
	
	poss = q;
	//poss = (poss << 8) + w; 
	return poss;
}

float nilai_coep (int loc_pless, float temp)
{
	float suhux;
	float cuhu1,cuhu2,low_cuhu;
	float koep;
	int n_suhu,n_ples;
	
	st_astm = pvPortMalloc( sizeof (struct t_astm) );
	if (st_astm == NULL)	{
		printf(" %s(): ERR allok memory gagal !\r\n", __FUNCTION__);
		vPortFree(st_astm);
		return 2;
	}
	printf("  %s(): Mallok @ %X\r\n", __FUNCTION__, st_astm);
	memcpy((char *) st_astm, (char *) (ALMT_VALUE_ASTM + (n_ples*364) + (n_suhu*sizeof(low_cuhu))), (2*sizeof (struct t_astm)));
	
	suhux = temp;
	n_suhu = lok_suhu(suhux);
	n_ples = loc_pless;
	
	#if 0
	IAP_return_t iap_return;
	iap_return = iapReadBlankSector(SEKTOR_ENV, SEKTOR_ENV);
	if (iap_return.ReturnCode == CMD_SUCCESS)	{
		printf("  ERR: Sektor Kosong !\r\n");
		return;
	}
	#endif
	
	st_astm = (char *) (ALMT_VALUE_ASTM + (n_ples*512) + (n_suhu*sizeof(low_cuhu)));
	cuhu1 = st_astm[0].koef;
	cuhu2 = st_astm[1].koef;
	//cuhu2 = st_astm;
	
	//low_cuhu = ((n_suhu-1) /4) +25;
	low_cuhu = n_suhu/4 +25;
	
	/*INTERPOLASI*/
	koep = ((suhux-low_cuhu)*(cuhu2-cuhu1)*4)+cuhu1;
	vPortFree (st_astm);
	
	return koep;
}







