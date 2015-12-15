/*
	Afrendy Bayu
	23 Mei 2012

*/

#include "FreeRTOS.h"
#include "task.h"
//#include "serial/tinysh.h"
//#include "sh_serial.h"
#include "monita.h"
#include "sh_rtc.h"
#include "hardware.h"


#ifdef PAKAI_RTC

#ifndef __M_RTC__
#define __M_RTC__


//extern unsigned int uptime_ovflow;

void set_date_kitab()	{
	qsprintf(" set_date tahun bulan tanggal jam menit hari\r\n");
	qsprintf(" set_date [tahun] [1-12] [1-31] [0-23] [0-59] [1-7]\r\n");
	qsprintf("   misalnya : set_date 2010 3 5 10 22\r\n");
	qsprintf("   artinya  : set waktu ke tgl 5 Maret 2010, jam 10:22 pagi\r\n");	
}

#if 0
void cek_uptime_modul()	{
	extern unsigned int tot_idle;
	
	int nW[5];
	portTickType w;
	portTickType qaz = (portTickType) (portMAX_DELAY/configTICK_RATE_HZ);
	
	w = (xTaskGetTickCount()/configTICK_RATE_HZ) + (uptime_ovflow*qaz);
	//printf(" TickCount : %d : %d : %d, ov: %d\r\n", xTaskGetTickCount(), w, qaz, uptime_ovflow );
	
	hitung_wkt(w, &nW);
	printf(" Up  = ");
	if (nW[4] !=0)
		printf("%d thn ", nW[4]);
	if (nW[3] !=0)
		printf("%d hari ", nW[3]);		
	if (nW[2] !=0)
		printf("%d jam ", nW[2]);		
	if (nW[1] !=0)
		printf("%d mnt ", nW[1]);		
	printf("%d dtk : idle = %d\r\n", nW[0], tot_idle);
	
	#ifdef PAKAI_RTC
		get_cal();
	#endif
	
}

tinysh_cmd_t uptime_cmd={0,"uptime","lama running","[args]",  cek_uptime_modul,0,0,0};
#endif

void set_date(int argc, char **argv)	{	
	unsigned char *str_rtc;
	struct rtc_time tmku;
	int ret;
	time_t clk;	
	
	str_rtc = pvPortMalloc(512);
	if (str_rtc == NULL) {
		qsprintf("ERR: alok failed\r\n");
		//free(str_rtc);
		vPortFree(str_rtc);
		return;
	}
	
	if (argc < 6) 	{
		qsprintf("Argument kurang !\r\n");
		vPortFree(str_rtc);
		set_date_kitab();
		return;
	}
		
	//display_args(argc, argv);
	sprintf(str_rtc, "%s:%s:%s:%s:%s:%s", argv[1], argv[2], argv[3], argv[4], argv[5], argv[6]); 
	ret = sscanf(str_rtc, "%d:%d:%d:%d:%d:%d", 			\
		&tmku.tm_year, &tmku.tm_mon, &tmku.tm_mday, 	\
		&tmku.tm_hour, &tmku.tm_min, &tmku.tm_wday); 
	if (ret < 5)	{
		printf(" ERR: format salah !\r\n");
		vPortFree(str_rtc);
		return;
	}

	qsprintf(" Set : %d-%d-%d %d:%d ",  tmku.tm_year, tmku.tm_mon, tmku.tm_mday, tmku.tm_hour, tmku.tm_min); 
		
	tmku.tm_year = tmku.tm_year - 1900;
	tmku.tm_mon  = tmku.tm_mon - 1;
	tmku.tm_sec  = 0;
	tmku.tm_wday = tmku.tm_wday - 1;

	rtcWrite( &tmku );
	//clk = mktime(&tmku);	
	//ret = rtc_time_to_bfin(clk);
	//bfin_write_RTC_STAT(ret);
	vPortFree(str_rtc);
	qsprintf(" ..OK\r\n");
	//*/
}						 
		
void cek_flag_RTC_sh()	{
	//printf(" flagRTCc: %d\r\n", flagRTCc);
}

void init_RTC_sh()	{
	setup_rtc();
	start_rtc();
	uprintf(" ********* init RTC: %d\r\n", flagRTCc);
}

void start_uptime()		{
	struct tm *a;
	st_hw.wkt_awal = (unsigned int) now_to_time(1, a);		// epoch
}

void hitung_uptime()	{
	unsigned int beda_t;
	struct tm *a;
	int nW[5];
	char kalimat[100], katakan[30];
	
	st_hw.wkt_now = (unsigned int) now_to_time(1, a);		// epoch
	beda_t = st_hw.wkt_now - st_hw.wkt_awal;
	if (beda_t<0)	{
		uprintf("  \r\n KALENDER ERROR !!!\r\n");
		return 0;
	}
	
	hitung_wkt(beda_t, &nW);
	
	strcpy(kalimat, "");
	if (nW[4] > 0)	{
		sprintf(katakan," %d thn ", nW[4]);
		strcat(kalimat, katakan);
	}
	if (nW[3] > 0)	{
		sprintf(katakan, "%d hari ", nW[3]);
		strcat(kalimat, katakan);
	}
	if (nW[2] > 0)	{
		sprintf(katakan, "%d jam ", nW[2]);
		strcat(kalimat, katakan);
	}
	if (nW[1] > 0)	{
		sprintf(katakan, "%d mnt ", nW[1]);
		strcat(kalimat, katakan);
	}
	
	a = localtime (&st_hw.wkt_awal);
	uprintf("\r\n  UPtime %s%d dtk [total %d detik]\r\n  Sejak %d-%02d-%4d %d:%02d%:%02d ", kalimat, nW[0], beda_t,	\
		a->tm_mday, a->tm_mon+1, a->tm_year+1900, a->tm_hour, a->tm_min, a->tm_sec);
		
	a = localtime (&st_hw.wkt_now);
	uprintf(  " Now: %d-%02d-%4d %d:%02d%:%02d\r\n", \
		a->tm_mday, a->tm_mon+1, a->tm_year+1900, a->tm_hour, a->tm_min, a->tm_sec);
}

void get_cal()	{
	//char* hari[] = {"Senin", "Selasa", "Rabu", "Kamis", "Jumat", "Sabtu", "Minggu"};
	char* bln[] = {"", "Jan", "Feb", "Mar", "Apr", "Mei", "Jun", "Jul", "Agt", "Sep", "Okt", "Nov", "Des"};
	rtcCTIME0_t ctime0;
	rtcCTIME1_t ctime1;
	rtcCTIME2_t ctime2;
	ctime0.i = RTC_CTIME0; 
	ctime1.i = RTC_CTIME1; 
	ctime2.i = RTC_CTIME2;
	
	#if 0
	qsprintf(	"\r\n  Waktu : %s, %d-%s-%04d %d:%02d:%02d\r\n", 			\
		hari[ctime0.dow], ctime1.dom, bln[ctime1.month], ctime1.year, 	\
		ctime0.hours, ctime0.minutes, ctime0.seconds);		// ctime2.doy, 
	#endif
	#if 1
	qsprintf(	"\r\n  Waktu : %d-%s-%04d %d:%02d:%02d\r\n", 			\
		ctime1.dom, bln[ctime1.month], ctime1.year, 	\
		ctime0.hours, ctime0.minutes, ctime0.seconds);		// ctime2.doy, 
	#endif
}

void cek_rtc_mem(int argc, char ** argv)	{
	uprintf("\r\n");
	int jml, mulai, cc, i;
	float kf;
	if (argc==3)	{
		mulai = atoi(argv[1]);
		if (mulai==0)		{
			uprintf("\r\n  Arg Register mulai SALAH !!!\r\n");
		}
		jml = atoi(argv[2]);
		if (jml>50)	jml=50;
		if (jml==0)		{
			uprintf("\r\n  Arg Jml SALAH !!!\r\n");
		}
		for (i=0; i<jml; i++)	{
			kf = *( (float*) &(*(&MEM_RTC0+(RTC_MEM_START+mulai+i))));
			uprintf("  Nilai[%2d]: %.2f, dataf[%d]: %.2f\r\n", mulai+i, kf, i, data_f[i]);
		}
	}
	if (argc==2)	{
		mulai = atoi(argv[1]);
		for (i=0; i<JML_KANAL; i++)	{
			kf = *( (float*) &(*(&MEM_RTC0+(RTC_MEM_START+mulai+i))));
			uprintf("  Nilai[%2d]: %.2f\r\n", mulai+i, kf);
		}
	}
}

void set_rtc_mem(int argc, char ** argv)	{
	uprintf("\r\n");
	if (argc!=3)	{
		rtc_kitab(0);
		return 1;
	}
	
	int nx = atoi(argv[1]);			// nx valid >0
	float fl, kf;
	unsigned int *ifl;
	
	if (nx==0)	{
		rtc_kitab(1);
		return 1;
	}
	sscanf(argv[2], "%f", &fl);
	uprintf("  memRTC [%d]: %.3f\r\n", nx, fl);
	
	ifl = (unsigned int *) &fl;
	if (nx<71) data_f[nx-1] = fl;
	else astm_f[nx-71] = fl;

	*(&MEM_RTC0+(RTC_MEM_START+nx)) = *( (int*) &fl);	// 100 + kanal (1-...)
	
	kf = *( (float*) &(*(&MEM_RTC0+(RTC_MEM_START+nx))));
	uprintf("  memRTC [%d/%d]: %.3f, %d\r\n", nx, RTC_MEM_START+nx, kf, ifl);
	
	#if 1
	struct t_env *st_env;
	st_env = ALMT_ENV;
	if (nx>10)	return;
	
	int ch = nx-1;
	char status=st_env->kalib[ch].status;
	if (status==sRUNNING_HOURS)		{
		konter.t_konter[ch].rh_x = (int) fl;
		uprintf("   data[%d]= %.f, %d\r\n", ch, data_f[ch], konter.t_konter[ch].rh_x);
	}
	else if (status==sFLOWx)		{
		konter.t_konter[ch].hit = (int) (fl-st_env->kalib[ch].C)/st_env->kalib[ch].m;
		uprintf("   data[%d]= %.f, %d\r\n", ch, data_f[ch], konter.t_konter[ch].hit);
	}
	#endif
}

void set_rtc_mem_default()	{
	float fl=0;
	int i;

	for (i=0; i<JML_TITIK_DATA; i++)	{
		*(&MEM_RTC0+(RTC_MEM_START+i)) = *( (int*) &fl);
	}
}

#ifdef PAKAI_KONTROL_RTC

void rtc_kitab(char *x)	{
	printf("  %s [waktu]\r\n", x);
	printf("    contoh : %s [1|detik | 2|menit | 3|jam]\r\n", x);
	//printf("    %s \r\n", x);
}

void cek_rtc_irq_sh(int argc, char **argv) {
	printf(" Cek Status IRQ RTC\r\n");
	printf("   ILR	  : 0x%04x\r\n", RTC_ILR);
	printf("   CTC	  : %d\r\n", (RTC_CTC>>1));
	printf("   CIIR	  : 0x%04x\r\n", RTC_CIIR);
	printf("   AMR	  : 0x%04x\r\n", RTC_AMR);
	printf("   CCR	  : 0x%04x\r\n", RTC_CCR);
//	printf("   CTIME0 : 0x%04x\r\n", RTC_CTIME0);
//	printf("   CTIME1 : 0x%04x\r\n", RTC_CTIME1);
//	printf("   CTIME2 : 0x%04x\r\n", RTC_CTIME2);
	get_cal();
	

}

//static tinysh_cmd_t cek_irq_rtc_cmd={0,"cek_irqrtc","cek status IRQ rtc", "help default ",cek_rtc_irq_sh,0,0,0};

void set_rtc_counter_irq_sh(int argc, char **argv) {
	display_args(argc, argv);
	if ( (argc<2) || (argc>2) )	{	// 
		rtc_kitab(argv[0]);
		return 0;
	}
	
	int aaa;
	unsigned char almx;
	almx = (unsigned char) atoi(argv[1]);
	
	printf("  Set interrupt RTC counter : 0x%02x\r\n", almx);
	aaa = rtc_counter_irq_aktif(almx);
	printf(" setting : 0x%04x, CCR: 0x%04x\r\n", aaa, RTC_CCR);
	*(&MEM_RTC0+(100)) = almx;
}

//static tinysh_cmd_t set_irq_rtcc_cmd={0,"set_irqrtc","set mode power", "help default ",set_rtc_counter_irq_sh,0,0,0};

void set_rtc_alarm_irq_sh(int argc, char **argv) {
	display_args(argc, argv);
	if ( (argc<2) || (argc>2) )	{	// 
		rtc_kitab(argv[0]);
		return 0;
	}

	int aaa;
	unsigned char almx;
	almx = (unsigned char) atoi(argv[1]);
	
	printf("  Set interrupt RTC alarm : 0x%02x\r\n", almx);
	aaa = rtc_alarm_aktif(almx);
	printf(" setting : 0x%04x, CCR: 0x%04x\r\n", aaa, RTC_CCR);
}

static tinysh_cmd_t set_irq_rtca_cmd={0,"set_rtc_irqa","set status alarm RTC", "help default ",set_rtc_alarm_irq_sh,0,0,0};


void cek_waktu_alarm_sh()	{
	printf("ALM tgl   : %d:%02d:%02d\r\n", RTC_ALDOM, RTC_ALMON, RTC_ALYEAR);
	printf("ALM waktu : %d:%02d:%02d\r\n", RTC_ALHOUR, RTC_ALMIN, RTC_ALSEC);
	printf(" Status 0x%02x\r\n", RTC_AMR);
}
static tinysh_cmd_t cek_waktu_alarm_cmd={0,"cek_waktu_alarm","cek waktu alarm RTC", "help default ",cek_waktu_alarm_sh,0,0,0};

void set_waktu_alarm_sh(int argc, char **argv)	{
	display_args(argc, argv);
	if ( (argc<4) || (argc>4) )	{	// 
		rtc_kitab(argv[0]);
		return 0;
	}
	
	unsigned char jam, min, det;
	RTC_ALHOUR = jam = (unsigned char) atoi(argv[1]);
	RTC_ALMIN  = min = (unsigned char) atoi(argv[2]);
	RTC_ALSEC  = det = (unsigned char) atoi(argv[3]);
}
static tinysh_cmd_t set_waktu_alarm_cmd={0,"set_waktu_alarm","set waktu alarm RTC", "help default ",set_waktu_alarm_sh,0,0,0};


#endif

#endif

#endif
