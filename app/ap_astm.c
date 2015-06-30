

// Jefri Sihombing
// Depok, 13 April 2015
// 


#include "FreeRTOS.h"
#include "task.h"
#include "monita.h"
#include "ap_astm.h"
#include "hardware.h"
#include "math.h"

//struct t_astm *st_astm;

int lok_suhu(float cuhu)
{
	int poss,q;
	float cuhu_temp;
	
	//pless_temp = pless_temp+0.5;
	//q = (int) pless_temp;
	
	cuhu_temp = cuhu-25.00;
	//printf("suhu = %f",cuhu_temp);
	//cuhu_temp = cuhu_temp*4 +1;
	cuhu_temp = cuhu_temp*4;
	q = (int) cuhu_temp;
	//w = w+1;
	printf("suhu = %d",q);
	
	poss = q;
	//poss = (poss << 8) + w; 
	return poss;
}

float nilai_coep (int loc_pless, int temp)
{
	int n_ples;
	float suhux;
	float cuhu1,cuhu2,low_cuhu;
	float koep;
	int n_suhu;

	struct t_astm *st_astm;	
	st_astm = pvPortMalloc( PER_ASTM * sizeof (struct t_astm) );
	if (st_astm == NULL)	{
		printf(" %s(): ERR allok memory gagal !\r\n", __FUNCTION__);
		vPortFree(st_astm);
		return 2;
	}
	//printf("lappet = %f",lappet);

	suhux = data_f[temp+6];
	//printf("temp = %f",suhux);
	n_suhu = lok_suhu(suhux);
	//printf("nples = %d",loc_pless);
	n_ples = (int) (loc_pless-810)/2;
	//printf("nples = %d",n_ples);
	memcpy((char *) st_astm, (char *) ALMT_VALUE_ASTM+(n_ples*JML_KOPI_ASTM), (PER_ASTM * sizeof (struct t_astm)));	
	cuhu1 = st_astm[n_suhu].koef;
	printf("k1 = %f",cuhu1);
	cuhu2 = st_astm[n_suhu+1].koef;
	printf("k2 = %f",cuhu2);
	//cuhu2 = st_astm;
	
	//low_cuhu = ((n_suhu-1) /4) +25;
	low_cuhu =((float)n_suhu/4) +25.00;
	//printf("low_suhu = %f",low_cuhu);
	/*INTERPOLASI*/
	koep = (((suhux-low_cuhu)*(cuhu1-cuhu2)*4.00)+cuhu1);
	printf("koep = %f",koep);
	vPortFree (st_astm);
	
	return koep;
}







