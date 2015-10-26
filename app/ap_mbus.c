
#include "FreeRTOS.h"
#include "monita.h"
#include "ap_mbus.h"

#ifdef MOV_AVG

extern volatile float data_f[];

void data_mbus()
{
	int i;

	struct t_env *st_env;
	st_env = ALMT_ENV;	

	for (i=0; i<JUM_MBUS; i++)		
	{
		if (st_env->kalib[(2*JML_KANAL)+i].status == sMBUS)	
		{
			data_f[(2*JML_KANAL)+i] = (mbus.ke_0[i]+mbus.ke_1[i]+mbus.ke_2[i]+mbus.ke_3[i]+mbus.ke_4[i])/5;
			mbus.ke_4[i]=mbus.ke_3[i];
			mbus.ke_3[i]=mbus.ke_2[i];
			mbus.ke_2[i]=mbus.ke_1[i];
			mbus.ke_1[i]=mbus.ke_0[i];
		}
		else if (st_env->kalib[(2*JML_KANAL)+i].status == sMBUS2)
		{
			data_f[(2*JML_KANAL)+i] = mbus.ke_0[i];
		}
	}	
}
#endif
