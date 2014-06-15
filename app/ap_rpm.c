
#include "FreeRTOS.h"
#include "task.h"
#include "ap_rpm.h"
#include "monita.h"
#include "hardware.h"
#include "math.h"
#include <time.h>

extern unsigned int giliran;
//extern volatile float data_f[];
extern struct t2_konter konter;
//extern unsigned char status_konter[];


void cek_input_onoff(void)	{
	struct t_env *env2;
	env2 = (char *) ALMT_ENV;
	int i=0, ww=0, zzz;
	
	for (i=0; i<JML_KANAL; i++)	{
		ww = env2->kalib[i].status;
		if ( (ww==sONOFF) || (ww==sPUSHBUTTON)  || (ww==sONOFF_RH) || (ww==sFLOW1) || (ww==sFLOW2) || (ww==ssFLOW2) )	{
			switch (i)	{
				case 0: zzz = iKonter_1; break;
				case 1: zzz = iKonter_2; break;
				case 2: zzz = iKonter_3; break;
				case 3: zzz = iKonter_4; break;
				case 4: zzz = iKonter_5; break;
				case 5: zzz = iKonter_6; break;
				case 6: zzz = iKonter_7; break;
				case 7: zzz = iKonter_8; break;
				case 8: zzz = iKonter_9; break;
				case 9: zzz = iKonter_10; break;
			}
			if (i>=5) {
				konter.t_konter[i].onoff = PORT0_INPUT(zzz);
			} else {
				konter.t_konter[i].onoff = PORT2_INPUT(zzz);
			}
			/*
			if (i==2)	{
				uprintf("kanal 3: %d\r\n", konter.t_konter[i].onoff);
			}
			//*/
		}
	}
}

void set_konter_rpm(int st, unsigned int period)		{
	//new_period = T1TC;
	if (period > konter.t_konter[st].last_period)	{
		konter.t_konter[st].beda = (period -
			konter.t_konter[st].last_period) * 50;	// 1 clock 50 nanosecond
	}	else	{	// sudah overflow
		konter.t_konter[st].beda = (period +
			(0xFFFFFFFF - konter.t_konter[st].last_period)) * 50;	// 1 clock 50 nanosecond

	}
	konter.t_konter[st].hit++;
	konter.t_konter[st].last_period = period;
}

void set_konter_onoff(int i, int onoff) {	// hanya menambah jml counter hit tiap kanal saja
	//konter.t_konter[i].onoff = onoff;		// status dilihat dari GPIO (polling) saja
	konter.t_konter[i].hit++;
}

void reset_konter(void)	{
	int i;
	
	konter.global_hit = 0;
	konter.ovflow = 0;
	giliran = 0;
	
	for (i=0; i<JML_KANAL; i++)		{
		konter.t_konter[i].last_period = 0;
		konter.t_konter[i].beda = 0;
		konter.t_konter[i].hit = 0;
		konter.t_konter[i].hit2 = 0;
		konter.t_konter[i].hit_lama = 0;
		konter.t_konter[i].hit_lama2 = 0;
		konter.t_konter[i].onoff = 0;
		
		konter.t_konter[i].rh_on = 0;
		konter.t_konter[i].rh_off = 0;
		konter.t_konter[i].rh_flag = 0;
		konter.t_konter[i].rh = 0;
		
		//status_konter[i] = 0;
	}

}

void hitung_rpm(void)	{	
	//uprintf("%s() masuk ..., hit: %d, %d\r\n", __FUNCTION__, data_putaran[3], data_hit[3]);
	//uprintf("%s() masuk ..., hit: %d %d\r\n", __FUNCTION__, konter.t_konter[0].hit, konter.t_konter[1].hit);

	struct t_env *st_env;
	st_env = ALMT_ENV;
	int i;
	
	//printf("giliran: %d\r\n", giliran);
	
	char status = st_env->kalib[giliran].status;
	if (status==sPROP || status==sRPM || status == sRPM_RH)		{
		
		//portENTER_CRITICAL();
		
		if (konter.t_konter[giliran].hit_lama == konter.t_konter[giliran].hit)		{
			//konter.t_konter[giliran].beda = 0;		
			data_putaran[giliran] = 0;
			
		}	else	{
			/* didapatkan frekuensi (HZ) putaran */
			//temp_rpm = konter.t_konter[i].beda; // beda msh dlm nS
			
			/* rpm */
			data_putaran[giliran] = konter.t_konter[giliran].beda;
		}
		data_hit[giliran] = konter.t_konter[giliran].hit;
		
		konter.t_konter[giliran].hit_lama = konter.t_konter[giliran].hit; 
		#ifdef PAKAI_PILIHAN_FLOW
		
		konter.t_konter[giliran].hit_lama2 = konter.t_konter[giliran].hit2; 
		#endif
		//portEXIT_CRITICAL();
	}
	giliran++;
	if (giliran == JML_KANAL) giliran = 0;
}

#if 0
time_t now_to_time(int now, struct tm waktu)	{
	rtcCTIME0_t ctime0;
	rtcCTIME1_t ctime1;
	//rtcCTIME2_t ctime2;
	ctime0.i = RTC_CTIME0; 
	ctime1.i = RTC_CTIME1; 
	//ctime2.i = RTC_CTIME2;

	time_t t_of_day;
	
	if (now == 1)	{
		struct tm t;
		t.tm_year = ctime1.year - 1900;		// mulai 1900
		t.tm_mon = ctime1.month - 1;		// 0-11
		t.tm_mday = ctime1.dom;				// 1-31
		t.tm_hour = ctime0.hours;
		t.tm_min = ctime0.minutes;
		t.tm_sec = ctime0.seconds;
		t_of_day = mktime(&t);
	} else {	
		#if 0
		t.tm_year = 2013 - 1900;
		t.tm_mon = 10;
		t.tm_mday = 2;
		t.tm_hour = 17;
		t.tm_min = 27;
		t.tm_sec = 30;
		//t.tm_isdst = 1;
		#endif
		t_of_day = mktime(&waktu);
	}
	return t_of_day;
}
#endif

void hitung_running_hours(int i)		{
	time_t t;
	t = konter.t_konter[i].rh_off - konter.t_konter[i].rh_on;
	//printf("t: %d, off: %d, on: %d\r\n", t, konter.t_konter[i].rh_off, konter.t_konter[i].rh_on);
	konter.t_konter[i].rh = t;
	data_f[i] = konter.t_konter[i].rh_x + t;
	*(&MEM_RTC0+RTC_MEM_START+i+1) = *( (int*) &data_f[i]);
	//printf("rh[%d]: %.0f, rh[%d]: %.0f\r\n", 3, data_f[2], 10, data_f[9]);
	//data_f[28] = konter.t_konter[i].rh_x + t;
}

//int cobasini;// = 0;

void data_frek_rpm (void) {
	//qsprintf("%s() masuk ...\r\n", __FUNCTION__);
	unsigned int i=0;
	char status;
	float temp_f, fl2;
	float temp_rpm;
	
	struct t_env *st_env;
	st_env = ALMT_ENV;
	
	for (i=0; i<JML_KANAL; i++)	{
		status = st_env->kalib[i].status;
		
		if (status==sRPM || status==sRPM_RH)		{
			//*
			if (data_putaran[i])	{
				// cari frekuensi
				temp_f = (float) 1000000000.00 / data_putaran[i]; // beda msh dlm nS
				// rpm
				temp_rpm = temp_f * 60;		// ganti ke rps * 60;
				
			}
			else	{
				temp_f = 0;
				temp_rpm = 0;
			}
			#if 0
			if (i==1)	{
				uprintf("%s() masuk ...%d f: %.2f, rpm: %.2f\r\n", __FUNCTION__, data_putaran[i], temp_f, temp_rpm);
			}
			#endif

			#if 1
			//data_f[(i*2)+1] = (konter.t_konter[i].hit*st_env->kalib[i].m)+st_env->kalib[i].C;
			//data_f[i*2] = (float) (temp_rpm*st_env->kalib[i].m)+st_env->kalib[i].C;
			
			fl2 = (float) (temp_rpm*st_env->kalib[i].m)+st_env->kalib[i].C;
			if (fl2>RPM_MAX)	fl2 = 0;
			else	data_f[i] = fl2;
			#endif
			
			#ifdef PAKAI_RTC
			//*(&MEM_RTC0+(i*2+1)) = (int) data_f[i*2+1];	// konter.t_konter[i].hit;
			//*(&MEM_RTC0+RTC_MEM_START+i+1) = 0;
			#endif
			//*/
		}
		else if (status==sPROP)		{
			
			//*
			if (data_putaran[i])	{
				// cari frekuensi
				temp_f = (float) 1000000000.00 / data_putaran[i]; // beda msh dlm nS
				// rpm
				temp_rpm = temp_f * 60;		// ganti ke rps * 60;
			}
			else	{
				temp_f = 0;
				temp_rpm = 0;
			}
			

			#if 1
			fl2 = (float) (temp_rpm*st_env->kalib[i].m)+st_env->kalib[i].C;
			//printf("kanal: %d, rpm: %.2f, fl: %.2f\r\n", i+1, temp_rpm, fl2);
			if (fl2>PROP_MAX)	fl2 = 0;
			else	data_f[i] = fl2;
			#endif

		}
		else if (status==sFLOWx)	{
			//*
			data_f[i] = (konter.t_konter[i].hit*st_env->kalib[i].m)+st_env->kalib[i].C;
			
			#if 1
			if (data_f[i]>nFLOW_MAX) {		// reset setelah 10juta, 7 digit
			//if (data_f[(i*2)+1]>1000) {		// tes saja, reset setelah 10juta, 7 digit
				data_f[i] = 0;
				konter.t_konter[i].hit = 0;
			}
			#endif
			
			#ifdef PAKAI_RTC
			*(&MEM_RTC0+RTC_MEM_START+i+1)   = *( (int*) &data_f[i]);
			//*(&MEM_RTC0+(i*2))   = data_f[i*2];		// konter.t_konter[i].onoff;
			//*(&MEM_RTC0+(i*2+1)) = data_f[i*2+1];		// konter.t_konter[i].hit;
			#endif
			//*/
		}
		else if (status==sONOFF)	{
			data_f[i] = konter.t_konter[i].onoff;
		}
		else if (status==sONOFF_RH)	{
			//cobasini++;
			//uprintf("cobasin1: %d --- %d [%d]\r\n", konter.t_konter[i].onoff,cobasini, (konter.t_konter[i].onoff>0)?1:0);
			//uprintf("cobasin2: %d --- %d [%d]\r\n", konter.t_konter[i].onoff,cobasini, (konter.t_konter[i].onoff>0)?1:0);
			//data_f[29] = konter.t_konter[i].onoff;
			
			struct tm w;
			time_t t;
			//*
			t = now_to_time(1, w);
			//printf("       now_to_time: %d\r\n", t);
			int fx = konter.t_konter[i].rh_flag;
			if (konter.t_konter[i].onoff>0 && fx==0)	{		// rpm mutar dari mati
				konter.t_konter[i].rh_on = t;		// waktu mulai
				konter.t_konter[i].rh_flag = 1;
				//uprintf("----------> flag: 1  >>> %ld  -- %ld !!\r\n", konter.t_konter[i].rh, konter.t_konter[i].rh_x);
			}
			if (fx==1)	{		// rpm jalan
				konter.t_konter[i].rh_off = t;		// waktu berhenti
				hitung_running_hours(i);
				//uprintf("----------> flag: 1x >>> %ld  -- %ld !!\r\n", konter.t_konter[i].rh, konter.t_konter[i].rh_x);
			}
			if (konter.t_konter[i].onoff==0 && fx==1)		{			// rpm mati, simpan dulu
				konter.t_konter[i].rh_x += konter.t_konter[i].rh;
				konter.t_konter[i].rh_flag = 2;
				//uprintf("===========> flag: 2  >>> %ld  -- %ld !!\r\n", konter.t_konter[i].rh, konter.t_konter[i].rh_x);
			}
			if (fx==2)	{
				konter.t_konter[i].rh_flag = 0;
			}
			//printf("rh[%d]: %.0f, fx: %d, onoff: %d\r\n", i+1, data_f[i], fx, konter.t_konter[i].onoff);
			//*/
		}
		else if (status==sRUNNING_HOURS)	{
		/*
			struct tm w;
			time_t t;
			
			t = now_to_time(1, w);
			int fx = konter.t_konter[i].rh_flag;
			if (data_f[i-1]>0 && fx==0)	{		// rpm mutar dari mati
				konter.t_konter[i].rh_on = t;		// waktu mulai
				konter.t_konter[i].rh_flag = 1;
				//uprintf("----------> flag: 1  >>> %ld  -- %ld !!\r\n", konter.t_konter[i].rh, konter.t_konter[i].rh_x);
			}
			if (fx==1)	{		// rpm jalan
				konter.t_konter[i].rh_off = t;		// waktu berhenti
				hitung_running_hours(i);
				//uprintf("----------> flag: 1x >>> %ld  -- %ld !!\r\n", konter.t_konter[i].rh, konter.t_konter[i].rh_x);
			}
			if (data_f[i-1]==0 && fx==1)		{			// rpm mati, simpan dulu
				konter.t_konter[i].rh_x += konter.t_konter[i].rh;
				konter.t_konter[i].rh_flag = 2;
				//uprintf("===========> flag: 2  >>> %ld  -- %ld !!\r\n", konter.t_konter[i].rh, konter.t_konter[i].rh_x);
			}
			if (fx==2)	{
				konter.t_konter[i].rh_flag = 0;
			}
		//*/
		}
		
		
		#if 0
		else if (status==sONOFF || status==sFLOW1 || status==sFLOW2 || status==ssFLOW2) {		// OnOff
			// 					
			data_f[i*2] = (float) konter.t_konter[i].onoff;
			data_f[i*2+1] = (float) konter.t_konter[i].hit;
			
			#ifdef PAKAI_RTC
			*(&MEM_RTC0+(i*2))	 = data_f[i*2];		// konter.t_konter[i].onoff;
			*(&MEM_RTC0+(i*2+1)) = data_f[i*2+1];	// konter.t_konter[i].hit;
			#endif
		
		#ifdef PAKAI_PILIHAN_FLOW
		} else if (status==nFLOW1) {		// flow_setelah_selector
			data_f[i*2]   = (float) konter.t_konter[i].hit;
			data_f[i*2+1] = (float) konter.t_konter[i].hit2;
			
			#ifdef PAKAI_RTC
			*(&MEM_RTC0+(i*2))	 = data_f[i*2];			// konter.t_konter[i].hit;
			*(&MEM_RTC0+(i*2+1)) = data_f[i*2+1];		// konter.t_konter[i].hit2;
			#endif
			//printf("   %d : %d, %d : %d", (i*2), *(&MEM_RTC0+(i*2)), (i*2+1), *(&MEM_RTC0+(i*2+1)));
		} else if (status==nFLOW2) {		// Hitung flow_setelah_selector, dengan kalibrasi
			//data_f[i*2]   = (float) konter.t_konter[i].hit;
			//data_f[i*2+1] = (float) konter.t_konter[i].hit2;

			data_f[i*2]   = (konter.t_konter[i].hit*env2->kalib[i].m)+env2->kalib[i].C;
			data_f[i*2+1] = (konter.t_konter[i].hit2*env2->kalib[i].m)+env2->kalib[i].C;
			
			#if 1
			if (data_f[i*2]>9999999.99)	{		// 
				konter.t_konter[i].hit = 0;
			}
			
			if (data_f[i*2+1]>9999999.99)	{
				konter.t_konter[i].hit2 = 0;
			}
			#endif
			
			//*(&MEM_RTC0+(i*2))   = data_f[i*2];		// konter.t_konter[i].hit;
			//*(&MEM_RTC0+(i*2+1)) = data_f[i*2+1];		// konter.t_konter[i].hit2;
			#ifdef PAKAI_RTC
			*(&MEM_RTC0+(i*2))	 = konter.t_konter[i].hit;		// konter.t_konter[i].hit;
			*(&MEM_RTC0+(i*2+1)) = konter.t_konter[i].hit2;		// konter.t_konter[i].hit2;
			#endif
		} else if (status==fFLOW) {		// flow_setelah_selector = 203
			data_f[i*2]   = (float) konter.global_hit;
			data_f[i*2+1] = (float) konter.ovflow;
			
			#ifdef PAKAI_RTC
			*(&MEM_RTC0+(i*2))	 = konter.global_hit;
			*(&MEM_RTC0+(i*2+1)) = konter.ovflow;
			#endif
		#endif

		}
		#endif
	}
}
