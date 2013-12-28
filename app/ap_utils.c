
// Afrendy Bayu
// Depok, 29 Juli 2013
// 


#include "FreeRTOS.h"
#include "task.h"
#include "monita.h"
#include "ap_utils.h"

#ifdef PAKAI_RTC

int sync_waktu_modem(unsigned int x)	{
	uprintf("+++++> %s: %d\r\n", __FUNCTION__, x);
	
}

void hitung_wkt(unsigned int w, int *wx)	{
	int aW[] = {1, 60, 60, 24, 365, 1};
	char i=0;
	
	for (i=0; i<5; i++)	{
		wx[i] = w /= (int) aW[i];
		//printf("wx[%d]: %d, w:%d, aW[%d]\r\n",i,  wx[i], w, i, aW[i]);
		if (wx[i]>=aW[i+1] && i<4)
			wx[i] %= aW[i+1];
		//printf("wx[%d]: %d\r\n",i,  wx[i]);
	}
}

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

#if defined(PAKAI_FILE_SIMPAN)

#endif

#ifdef PAKAI_MODBUS






#endif
