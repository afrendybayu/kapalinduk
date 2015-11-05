
#include "FreeRTOS.h"
#include "monita.h"
#include "ap_adc.h"
#include "adc/ad7708.h"

#ifdef PAKAI_ADC_7708

extern struct t_adc adc;
extern volatile float data_f[];
extern struct t_mavg *st_mavg;
//extern float data_f[];
//extern struct t_env env;

void hitung_running_hours_adc(int i)		{
	time_t t;
	t = adc.rh_off[i] - adc.rh_on[i];
	//printf("t: %d, off: %d, on: %d\r\n", t, konter.t_konter[i].rh_off, konter.t_konter[i].rh_on);
	adc.rh[i] = t;
	data_f[JML_KANAL+i] = adc.rh_x[i] + t;
	*(&MEM_RTC0+RTC_MEM_START+i+JML_KANAL+1) = *( (int*) &data_f[JML_KANAL+i]);
	//printf("rh[%d]: %.0f, rh[%d]: %.0f\r\n", 3, data_f[2], 10, data_f[9]);
	//data_f[28] = konter.t_konter[i].rh_x + t;
}

int up;

void data_adc()	{
	char i;
	float tf;
	
	unsigned char nox;
	struct t_env *st_env;
	st_env = ALMT_ENV;
	
	struct t_data *st_data;
	st_data = ALMT_DATA+JML_KOPI_TEMP;
	
	
	#if 1
	up +=1;
	if (up==100) up = 0;
	#endif
	
	for (i=0; i<JML_KANAL_ADC; i++)		{
		tf = (float) (adc.data[i] * faktor_pengali_420 / 0xffff);
		tf = st_env->kalib[JML_KANAL+i].m * tf + st_env->kalib[JML_KANAL+i].C+up;
		
		if (st_env->kalib[JML_KANAL_ADC+i].status == sADC_7708)	
		{
			if (st_data[i].mv_avg == 1)
			{
				nox = st_data[i].no_ma;
				st_mavg[nox].ke_0 = tf;
			}
			else data_f[JML_KANAL+i] = tf;
		}
		else if (st_env->kalib[JML_KANAL_ADC+i].status == sADC_RH)	{
			//uprintf("data[%d] : %.2f\r\n", JML_KANAL+i+1, tf);
			struct tm w;
			time_t t;
			t = now_to_time(1, w);
				
			int fx = adc.rh_flag[i];
			if (tf>5 && fx==0)	{				// rpm mutar dari mati
				adc.rh_on[i] = t;		// waktu mulai
				adc.rh_flag[i] = 1;
				//uprintf("----------> flag: 1  >>> %ld  -- %ld !!\r\n", adc.rh[i], adc.rh_x[i]);
			}
			if (fx==1)	{		// rpm jalan
				adc.rh_off[i] = t;		// waktu berhenti
				hitung_running_hours_adc(i);
				//uprintf("----------> flag: 1x >>> %ld  -- %ld !!\r\n", adc.rh[i], adc.rh_x[i]);
			}
			if (tf<5 && fx==1)		{			// rpm mati, simpan dulu
				adc.rh_x[i] += adc.rh[i];
				adc.rh_flag[i] = 2;
				//uprintf("===========> flag: 2  >>> %ld  -- %ld !!\r\n", adc.rh[i], adc.rh_x[i]);
			}
			if (fx==2)	{
				adc.rh_flag[i] = 0;
			}
		}
	}
	//printf("|adc|\n\r");
}
#endif
