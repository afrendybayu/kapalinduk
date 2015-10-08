

// Afrendy Bayu
// Depok, 27 Sept 2013
// 


#include "FreeRTOS.h"
#include "task.h"
#include "ff/fatfs/ff.h"
#include "monita.h"
#include "ap_file.h"
#include <time.h>

#ifdef PAKAI_FILE_SIMPAN

char* strcmd_conf[]  = {"env", "kanal", "sumber", "data", "files", NULL};
char* strcmd_env[]   = {"nama", "sn", "idslave", "debug1", "debug2", NULL};
char* strcmd_kanal[] = {"kalib", "status", NULL};
char* strcmd_smbr[]  = {"nama", "ip", "alamat", "stack", NULL};
char* strcmd_data[]  = {"nama", "id", "satuan", "status", "batas", "formula", NULL};
char* strcmd_files[] = {"jumlah", "urut", NULL};

enum xcmd_conf	{
	CONF_ENV, CONF_KANAL, CONF_SUMBER, CONF_DATA, CONF_FILES
} cmd_conf		__attribute__ ((section (".usbram1")));

enum xcmd_conf_env	{
	ENV_NAMA, ENV_SN, ENV_IDSLAVE, ENV_DEBUG1, ENV_DEBUG2
} cmd_conf_env		__attribute__ ((section (".usbram1")));

enum xcmd_conf_kanal	{
	KNL_KALIB, KNL_STATUS
} cmd_conf_kanal		__attribute__ ((section (".usbram1")));

enum xcmd_conf_sumber	{
	SMBR_NAMA, SMBR_IP, SMBR_ALAMAT, SMBR_STACK
} cmd_conf_sumber		__attribute__ ((section (".usbram1")));

enum xcmd_conf_data	{
	DATA_NAMA, DATA_ID, DATA_SATUAN, DATA_STATUS, DATA_BATAS, DARA_FORMULA
} cmd_conf_data		__attribute__ ((section (".usbram1")));

enum xcmd_conf_files	{
	FILES_JUMLAH, FILES_URUT
} cmd_conf_files		__attribute__ ((section (".usbram1")));

struct sconf	{
	int cmd;
	int index;
	char conf[128];
}		__attribute__ ((section (".usbram1")));

struct sconf st_conf		__attribute__ ((section (".usbram1")));
//#define FORMAT_SIMPAN_STRING


void parsing_cmd_setting_utama(char *str)	{
	char *pch, *pch2;
	int ni=-1, i;
	int jmlIs = nIstilah(strcmd_conf);

	pch = (char*) strtok (str,"[]");
	for(i=0; i<jmlIs; i++)	{
		pch2 = strstr (str, strcmd_conf[i]);
		if (pch2 != NULL)	{
			//printf("%d, pch2: %s\r\n", i, pch2);
			st_conf.cmd = i;
		}
	}

	//printf ("isi: %d - %s, %d\r\n", st_conf.cmd, pch, jmlIs);
}

int parsing_cfg_env(int i, char *str)	{
	int nx, num;
	char tstr[32], *pch;
	
	pch = strchr(str, '=') + 1;
	pch = strtok(pch, "\r\n");
	
	struct t_env *st_env;
	st_env = pvPortMalloc( sizeof (struct t_env) );
	
	if (st_env==NULL)	{
		printf("  GAGAL alokmem !");
		vPortFree (st_env);
		return;
	}
	memcpy((char *) st_env, (char *) ALMT_ENV, (sizeof (struct t_env)));

	if (i==ENV_NAMA)	{
		do	{
			pch++;
		} while (*pch==' ');
		//uprintf("   ENV nama:%s\r\n", pch);
		strcpy(st_env->nama_board, pch);
	}
	else if (i==ENV_SN)	{
		do	{
			pch++;
		} while (*pch==' ');
		//uprintf("   ENV SN  :%s\r\n", pch);
		strcpy(st_env->SN, pch);
	}
	else if (i==ENV_IDSLAVE)	{
		sscanf(str, "idslave = %d", &nx);
		uprintf("   ENV idslave:%d\r\n", nx);
		st_env->almtSlave = nx;
	}
	else if (i==ENV_DEBUG1)	{
		sscanf(str, "debug1 = %d", &nx);
		uprintf("   ENV debug1:%d\r\n", nx);
		st_env->prioDebug  = nx;
	}
	else if (i==ENV_DEBUG2)	{
		sscanf(str, "debug2 = %d", &nx);
		uprintf("   ENV debug2:%d\r\n", nx);
		st_env->prioDebug2  = nx;
	}
	
	simpan_st_rom(SEKTOR_ENV, ENV, 0, (unsigned short *) st_env, 0);
	vPortFree (st_env);
}

int parsing_cfg_kanal(int i, char *str)	{
	char *pch, *pch2;
	int nx, st;
	float m,c;
	
	struct t_env *st_env;
	st_env = pvPortMalloc( sizeof (struct t_env) );
	
	if (st_env==NULL)	{
		printf("  GAGAL alokmem !");
		vPortFree (st_env);
		return;
	}
	memcpy((char *) st_env, (char *) ALMT_ENV, (sizeof (struct t_env)));
	
	if (i==KNL_KALIB)	{
		sscanf(str, "kalib%d = %f %f", &nx, &m, &c);
		uprintf("   KALIB  [%d] m:%.3f, c:%.3f\r\n", nx, m, c);
		st_env->kalib[nx - 1].m = m;
		st_env->kalib[nx - 1].C = c;
	}
	else if (i==KNL_STATUS)	{
		sscanf(str, "status%d = %d", &nx, &st);
		uprintf("   STATUS [%d] st:%d\r\n", nx, st);
		st_env->kalib[nx - 1].status = st;
	}
	
	simpan_st_rom(SEKTOR_ENV, ENV, 0, (unsigned short *) st_env, 0);
	vPortFree (st_env);
}

int parsing_cfg_sumber(int i, char *str)	{
	int nx, num;
	char tstr[32], *pch;
	
	pch = strchr(str, '=') + 1;
	pch = strtok(pch, "\r\n");
	
	struct t_sumber *st_sumber;
	st_sumber = pvPortMalloc( JML_SUMBER * sizeof (struct t_sumber) );
	if (st_sumber == NULL)	{
		printf(" %s(): ERR allok memory gagal !\r\n", __FUNCTION__);
		vPortFree(st_sumber);
		return -1;
	}
	memcpy((char *) st_sumber, (char *) ALMT_SUMBER, (JML_SUMBER * sizeof (struct t_sumber)));
	
	if (i==SMBR_NAMA)	{
		sscanf(str, "nama%d = ", &nx);
		do	{
			pch++;
		} while (*pch==' ');
		uprintf("   SUMBER [%d] nama:%s\r\n", nx, pch);
	}
	else if (i==SMBR_IP)	{
		sscanf(str, "ip%d = ", &nx);
		do	{
			pch++;
		} while (*pch==' ');
		uprintf("   SUMBER [%d] ip:%s\r\n", nx, pch);
	}
	else if (i==SMBR_ALAMAT)	{
		sscanf(str, "alamat%d = %d", &nx, &num);
		uprintf("   SUMBER [%d] alamat:%d\r\n", nx, num);
	}
	else if (i==SMBR_STACK)		{
		sscanf(str, "stack%d = %d", &nx, &num);
		uprintf("   SUMBER [%d] stack:%d\r\n", nx, num);		
	}
	
	simpan_st_rom(SEKTOR_ENV, SUMBER, 1, (unsigned short *) st_sumber, 0);
	vPortFree(st_sumber);
}

int parsing_cfg_data(int i, char *str)	{
	int nx, num;
	char tstr[32], *pch;
	float rL, aLL, aL, aH, aHH, rH;
	int nox, lok;
	
	pch = strchr(str, '=') + 1;
	pch = strtok(pch, "\r\n");
	
	struct t_data *st_data;		
	st_data = pvPortMalloc( PER_SUMBER * sizeof (struct t_data) );
	if (st_data == NULL)	{
		printf(" %s(): ERR allok memory gagal !\r\n", __FUNCTION__);
		vPortFree (st_data);
		return 3;
	}

	
	if (i==DATA_NAMA)	{
		sscanf(str, "nama%d = ", &nx);
		do	{
			pch++;
		} while (*pch==' ');
		//uprintf("   DATA [%d] nama:%s\r\n", nx, pch);
		nx--;
		nox = nx % PER_SUMBER;
		lok = (int) (nx/PER_SUMBER);
		memcpy((char *) st_data, (char *) ALMT_DATA+(lok*JML_KOPI_TEMP), (PER_SUMBER * sizeof (struct t_data)));
		strcpy(st_data[nox].nama, pch);
	}
	else if (i==DATA_ID)	{
		sscanf(str, "id%d = %d", &nx, &num);
		//uprintf("   DATA [%d] id:%d\r\n", nx, num);
		nx--;
		nox = nx % PER_SUMBER;
		lok = (int) (nx/PER_SUMBER);
		memcpy((char *) st_data, (char *) ALMT_DATA+(lok*JML_KOPI_TEMP), (PER_SUMBER * sizeof (struct t_data)));
		st_data[nox].id = num;
	}
	else if (i==DATA_SATUAN)	{
		sscanf(str, "satuan%d = ", &nx);
		do	{
			pch++;
		} while (*pch==' ');
		//uprintf("   DATA [%d] satuan:%s\r\n", nx, pch);
		nx--;
		nox = nx % PER_SUMBER;
		lok = (int) (nx/PER_SUMBER);
		memcpy((char *) st_data, (char *) ALMT_DATA+(lok*JML_KOPI_TEMP), (PER_SUMBER * sizeof (struct t_data)));
		strcpy(st_data[nox].satuan, pch);
	}
	else if (i==DATA_STATUS)	{
		sscanf(str, "status%d = ", &nx);
		do	{
			pch++;
		} while (*pch==' ');
		num = (strcmp(pch, "MATI"))?0:1;
		//uprintf("   DATA [%d] status: [%d] %s\r\n", nx, num, pch);
		nx--;
		nox = nx % PER_SUMBER;
		lok = (int) (nx/PER_SUMBER);
		memcpy((char *) st_data, (char *) ALMT_DATA+(lok*JML_KOPI_TEMP), (PER_SUMBER * sizeof (struct t_data)));
		st_data[nox].status = num;
	}
	else if (i==DATA_BATAS)	{
		sscanf(str, "batas%d = %f %f %f %f %f %f", &nx, &rL, &aLL, &aL, &aH, &aHH, &rH);
		//uprintf("   DATA [%d] rL:%.1f, aLL:%.1f, aL:%.1f, aH:%.1f, aHH:%.1f, rH:%.1f\r\n", \
		//	nx, rL, aLL, aL, aH, aHH, rH);
		nx--;
		nox = nx % PER_SUMBER;
		lok = (int) (nx/PER_SUMBER);
		memcpy((char *) st_data, (char *) ALMT_DATA+(lok*JML_KOPI_TEMP), (PER_SUMBER * sizeof (struct t_data)));
		st_data[nox].rangeL = rL;
		st_data[nox].batasLL = aLL;
		st_data[nox].batasL  = aL;
		st_data[nox].batasH  = aH;
		st_data[nox].batasHH = aHH;
		st_data[nox].rangeH = rH;
	}
	
	simpan_st_rom(SEKTOR_DATA, lok, 1, (unsigned short *) st_data, 1);
	vPortFree (st_data);
}

int parsing_cfg_files(int i, char *str)	{
	int nx, num;
	char tstr[32], *pch;
	float rL, aLL, aL, aH, aHH, rH;
	
	pch = strchr(str, '=') + 1;		//if (pch==NULL)	return 1;
	pch = strtok(pch, "\r\n");		//if (pch==NULL)	return 2;
	
	struct t_file *st_file;
	st_file = pvPortMalloc( sizeof (struct t_file) );
	if (st_file == NULL)	{
		uprintf(" %s(): ERR allok memory gagal !\r\n", __FUNCTION__);
		vPortFree (st_file);
		return 2;
	}
	//printf("  %s(): Mallok @ %X\r\n", __FUNCTION__, st_file);
	memcpy((char *) st_file, (char *) ALMT_FILE, (sizeof (struct t_file)));
	
	if (i==FILES_JUMLAH)	{
		sscanf(str, "jumlah = %d", &nx);
		do	{
			pch++;
		} while (*pch==' ');
		//uprintf("   FILES jml:%d\r\n", nx);
		st_file->jml = nx;
	}
	else if (i==FILES_URUT)	{
		sscanf(str, "urut%d = ", &nx);
		num = atoi(pch);
		
		if (num>0)	{
		//if ((num>0) && ())	{
		//	uprintf("   FILES [%2d] urut:%d\r\n", nx, num);
			st_file->urut[nx-1] = num;
		}
	}
	
	simpan_st_rom(SEKTOR_ENV, BERKAS, 1, (unsigned short *) st_file, 0);
	vPortFree (st_file);
}

int parsing_cmd_setting_subutama(char *str)	{
	int jmlIs, nn=-1, i, nx;
	char *pch, *pch2, *pch3, *pch4;
	
	if (st_conf.cmd==CONF_ENV)	{
		jmlIs = nIstilah(strcmd_env);
		for(i=0; i<jmlIs; i++)	{
			pch = strstr (str, strcmd_env[i]);
			if (pch != NULL)	{
				parsing_cfg_env(i, str);
			}
		}
	}
	else if (st_conf.cmd==CONF_KANAL)	{
		jmlIs = nIstilah(strcmd_kanal);
		for(i=0; i<jmlIs; i++)	{
			pch = strstr (str, strcmd_kanal[i]);
			if (pch != NULL)	{
				parsing_cfg_kanal(i, str);
			}
		}
	}
	else if (st_conf.cmd==CONF_SUMBER)	{
		jmlIs = nIstilah(strcmd_smbr);
		for(i=0; i<jmlIs; i++)	{
			pch = strstr (str, strcmd_smbr[i]);
			if (pch != NULL)	{
				parsing_cfg_sumber(i, str);
			}
		}
	}
	else if (st_conf.cmd==CONF_DATA)	{
		jmlIs = nIstilah(strcmd_data);
		for(i=0; i<jmlIs; i++)	{
			pch = strstr (str, strcmd_data[i]);
			if (pch != NULL)	{
				parsing_cfg_data(i, str);
			}
		}
	}
	else if (st_conf.cmd==CONF_FILES)	{
		jmlIs = nIstilah(strcmd_files);
		for(i=0; i<jmlIs; i++)	{
			pch = strstr (str, strcmd_files[i]);
			if (pch != NULL)	{
				parsing_cfg_files(i, str);
			}
		}
	}
	//printf ("----- %d/%d, %s", st_conf.cmd, jmlIs, str);
	return 0;
}

void parsing_file_setting(char *str)	{
	char *pch, *pch2;
	int n;
	
	pch = strchr(str, '#');		// komentar
	if (pch != NULL)	{
		n = pch-str+1;
		if (n<10)	{
			//printf(" n: %d, komentar; %s", n, str);
			return;
		}
	}
	
	pch = strchr(str, '[');
	if (pch != NULL)	{
		parsing_cmd_setting_utama(str);
		//uprintf ("NOT NILL cmd: %d\r\n", st_conf.cmd);
	} else {
		parsing_cmd_setting_subutama(str);
	}
}

int upload_konfig(char *path)	{
	FIL filx;
	FRESULT res;
	char strfile[128];
	
	res = f_open(&filx, path, FA_OPEN_EXISTING | FA_READ);
	if (res != FR_OK)	{
		uprintf("=== TIDAK BISA BUKA FILE ====\r\n");
		return 1;
	}

	while ( f_gets (strfile , 128 , &filx) != NULL )	{
		parsing_file_setting(strfile);
		vTaskDelay(1);
		//if (baris==20) break;
		//baris++;
	}
	
	//uprintf("=== AKHIR FILE ====\r\n");

	f_close(&filx);
	//uprintf("=== TUTUP FILE ====\r\n");
}

char *pisah_nf(char *pnf)	{
	char *pch, pcx[64], stmp[128];
	
	strcpy(stmp, pnf);
	//printf("--> %s() path: %s\r\n", __FUNCTION__, stmp);
	pch = strtok (stmp,"\\");
	while (pch != NULL)  {
		strcpy(pcx, pch);
		pch = strtok (NULL, "\\");
	}
	//printf("hasil: %s\r\n", pcx);
	return pcx;
}

void sendHexFile(int nilai, int jml, FIL fp)	{
	int i;
	char s[4];
	memcpy(s, (void*) &nilai, jml);
	f_write(&fp, s, jml, &i);
	
	#if 0
	for (i=0; i<jml; i++)	{
		fputc(s[i], fp);
	}
	#endif
}

void simpan_file_data()		{
	//if (st_hw.sdc==0) return;
	FIL filx;
	FRESULT res;
	DIR dir;
	char st[50], st2[50], s[4], fol[20];
	char isi[256];
	int i=0, oo, menit;
	unsigned int wx;
	int j, k;
	
	rtcCTIME0_t ctime0;
	rtcCTIME1_t ctime1;
	//rtcCTIME2_t ctime2;
	ctime0.i = RTC_CTIME0; 
	ctime1.i = RTC_CTIME1; 
	//ctime2.i = RTC_CTIME2;
	
	//uprintf("waktu: %d%02d%02d_%02d%02d\r\n", ctime1.year, ctime1.month, ctime1.dom, ctime0.hours, ctime0.minutes );
	menit =  (ctime0.minutes<30)?0:30;
	sprintf(fol, "0:\\%d%02d%02d", ctime1.year, ctime1.month, ctime1.dom);
	sprintf(st, "%s\\%d%02d%02d_%02d%02d.str", fol, ctime1.year, ctime1.month, ctime1.dom, ctime0.hours, menit);
	
	
	res = f_opendir(&dir, fol);
	//uprintf("res buka folder: %d\r\n", res);
	
	if (res != FR_OK)	{
		res = f_mkdir(fol);
		//uprintf("res buat folder: %d\r\n", res);
	}
	
	struct t_file  *st_file;
	st_file = (char *) ALMT_FILE;
	
	struct t_data *st_data;
	
	res = f_open(&filx, st, FA_OPEN_EXISTING | FA_READ | FA_WRITE);
	if (res)	{		// GAGAL buka, bikin baru !!!
		res = f_open(&filx, st, FA_OPEN_ALWAYS | FA_READ | FA_WRITE);
		if (res) 
		{
			uprintf("res: %d\r\n", res);
			return;
			//die(res);
		} 
		else 
		{
			// tambahan header untuk file baru !! -- disini --
			isi[0] = '\r'; isi[1] = '\n';
			for (i=0; i<st_file->jml; i++)	
			{
				j = (int) ((st_file->urut[i]-1)/PER_SUMBER);
				k = (st_file->urut[i]-1) % PER_SUMBER;
				st_data = ALMT_DATA + j*JML_KOPI_TEMP;
				
				memcpy((void*) &isi[4*i+2], (void*) &st_data[k].id, 4);
			}
			f_write(&filx, isi, (st_file->jml*4+2), &oo);
		}
	} 
	else 
	{		// file sudah ada
		res = f_lseek(&filx, f_size(&filx));
	}
	
	time_t wkt;
	struct tm a;
	//res = f_lseek(&filx, f_size(&filx));
	//f_write(&filx, "cobacoba\r\n", 10, &i);
	for (i=0; i<st_file->jml; i++)	{
		#ifdef FORMAT_SIMPAN_STRING
			if (i==0)	{
				sprintf(isi, "%ld ", now_to_time(1, a));
			}
			sprintf(st2, "%.1f ", data_f[st_file->urut[i]-1]);
			strcat(isi, st2);
		
		#else
			if (i==0)	{
				isi[0] = '\r'; isi[1] = '\n';
				wx = (unsigned int) now_to_time(1, a);
				memcpy(&isi[2], (void*) &wx, 4);
			}
			
			memcpy((void*) &isi[4*i+6], (void*) &data_f[st_file->urut[i]-1], 4);
			//f_write(&filx, s, 4, &oo);
		#endif

		//sendHexFile(data_f[10+i], 4, filx);
	}
	#ifdef FORMAT_SIMPAN_STRING
		//uprintf("%s", isi);
		f_puts(isi, &filx);
	#else
		f_write(&filx, isi, (st_file->jml*4+6), &oo);
	#endif
	
	//f_sync(&filx);
	f_close(&filx);
}

void tulis_konfig_file(char *s, FIL* fp)	{
	char ss[50];
	int jml, i;
	
	sprintf(ss, "%s\r\n", s);
	jml = strlen(ss);
	//uprintf("%s: jml: %d\r\n", ss, jml);
	f_write(fp, ss, jml, &i);
}

int simpan_konfig(int argc, char **argv)		{
	FIL filx;
	FRESULT res;
	char st1[50], st2[50], s[4];
	int i, j, oo;
	
	if (argc==1)	{
		//file_kitab(0);
		uprintf("\r\n  argc = 1\r\n");
		return;
	}
	
	uprintf("agv[1] : %s\r\n", argv[1]);
	rtcCTIME0_t ctime0;
	rtcCTIME1_t ctime1;
	ctime0.i = RTC_CTIME0; 
	ctime1.i = RTC_CTIME1; 
	
	sprintf(st2, "%d%02d%02d %02d:%02d:%02d", \
		ctime1.year, ctime1.month, ctime1.dom, ctime0.hours, ctime0.minutes, ctime0.seconds);
	res = f_open(&filx, argv[1], FA_OPEN_ALWAYS | FA_READ | FA_WRITE);
    if (res) {
		uprintf("\r\n  res: %d\r\n", res);
		return;
		//die(res);
	}
	
	struct t_env *st_env;
	st_env = ALMT_ENV;
	
	//uprintf("--> %s\r\n", __FUNCTION__);

	tulis_konfig_file("[env]", &filx);
	sprintf(st1, "#tanggal = %s", st2);
	tulis_konfig_file(st1, &filx);
	sprintf(st1, "nama = %s", st_env->nama_board);
	tulis_konfig_file(st1, &filx);
	sprintf(st1, "sn = %s", st_env->SN);
	tulis_konfig_file(st1, &filx);
	sprintf(st1, "idslave = %d", st_env->almtSlave);
	tulis_konfig_file(st1, &filx);
	sprintf(st1, "debug1 = %d", st_env->prioDebug);
	tulis_konfig_file(st1, &filx);
	sprintf(st1, "debug2 = %d", st_env->prioDebug2);
	tulis_konfig_file(st1, &filx);
	sprintf(st1, "#board = %s %s", PROMPT, BOARD_SANTER_versi);
	tulis_konfig_file(st1, &filx);
	tulis_konfig_file("", &filx);
	
	tulis_konfig_file("[kanal]", &filx);
	for (i=0; i<(JML_KANAL*2); i++)	{
		sprintf(st1, "kalib%d = %.3f %.3f", i+1, st_env->kalib[i].m, st_env->kalib[i].C);
		tulis_konfig_file(st1, &filx);
		sprintf(st1, "status%d = %d", i+1, st_env->kalib[i].status);
		tulis_konfig_file(st1, &filx);
	}
	tulis_konfig_file("", &filx);
	
	struct t_sumber *st_sumber;
	st_sumber = (char *) ALMT_SUMBER;
	
	tulis_konfig_file("[sumber]", &filx);
	for (i=0; i<JML_SUMBER; i++)	{
		sprintf(st1, "nama%d = %s", i+1, st_sumber[i].nama);
		tulis_konfig_file(st1, &filx);
		sprintf(st1, "ip%d = %d.%d.%d.%d", i+1, st_sumber[i].IP0, st_sumber[i].IP1, st_sumber[i].IP2, st_sumber[i].IP3);
		tulis_konfig_file(st1, &filx);
		sprintf(st1, "alamat%d = %d", i+1, st_sumber[i].alamat);
		tulis_konfig_file(st1, &filx);
		sprintf(st1, "stack%d = %d", i+1, st_sumber[i].stack);
		tulis_konfig_file(st1, &filx);
	}
	tulis_konfig_file("", &filx);
	
	struct t_data *st_data;
	tulis_konfig_file("[data]", &filx);
	for (i=0; i<JML_SUMBER; i++)	{
		st_data = ALMT_DATA + i*JML_KOPI_TEMP;
		
		for (j=0; j<PER_SUMBER; j++)	{
			sprintf(st1, "nama%d = %s", i*PER_SUMBER+j+1, st_data[j].nama);
			tulis_konfig_file(st1, &filx);
			sprintf(st1, "id%d = %d", i*PER_SUMBER+j+1, st_data[j].id);
			tulis_konfig_file(st1, &filx);
			sprintf(st1, "satuan%d = %s", i*PER_SUMBER+j+1, st_data[j].satuan);
			tulis_konfig_file(st1, &filx);
			sprintf(st1, "status%d = %s", i*PER_SUMBER+j+1, st_data[j].status?"Aktif":"Mati");
			tulis_konfig_file(st1, &filx);
			sprintf(st1, "batas%d = %d %d %d %d %d %d", i*PER_SUMBER+j+1, st_data[j].rangeL, \
					st_data[j].batasLL, st_data[j].batasL, 	\
					st_data[j].batasH, st_data[j].batasHH, st_data[j].rangeH);
			tulis_konfig_file(st1, &filx);
			sprintf(st1, "formula%d = %s", i*PER_SUMBER+j+1, st_data[j].formula);
			tulis_konfig_file(st1, &filx);
		}
	}
	tulis_konfig_file("", &filx);
	
	struct t_file  *st_file;
	st_file = (char *) ALMT_FILE;
	tulis_konfig_file("[files]", &filx);
	sprintf(st1, "jumlah = %d", st_file->jml);
	tulis_konfig_file(st1, &filx);
	for (i=0; i<JML_TITIK_DATA; i++)	{
		if (st_file->urut[i]==0)	break;
		sprintf(st1, "urut%d = %d", i+1, st_file->urut[i]);
		tulis_konfig_file(st1, &filx);
	}
	
	f_close(&filx);
}

void cari_waktu(char *dest, char *posisi) {		// cari posisi path folder
	
	if ((posisi[0]!='H') && (posisi[0]!='h') && (posisi[0]!='J') && (posisi[0]!='j') && (posisi[0]!='B') && (posisi[0]!='b') ) {
		uprintf("Argumen tidak benar !!\r\n");
		uprintf("Contoh : H-7, J-2, B-1\r\n");
		sprintf(dest,"\\");
		return;
	}

	struct tm *a;
	unsigned int wx = (unsigned int) now_to_time(1, a);		// epoch
	//uprintf("epoch : %ld\r\n", wx);
	char *pch, str[10];
	pch=strstr(posisi,"-");
  	if (pch!=NULL)
  		strcpy(str, pch+1);
  	else
  		return 0;
	
	//uprintf("waktu: %d%02d%02d_%02d%02d\r\n", ctime1.year, ctime1.month, ctime1.dom, ctime0.hours, ctime0.minutes );
	int ijam, itgl, ibulan, ithn, count;

	if ( (posisi[0]=='H') || (posisi[0]=='h') ) {
		wx -= (24*3600*atoi(str));
	}
	if ( (posisi[0]=='J') || (posisi[0]=='j') ) {
		wx -= (3600*atoi(str));
	}
	
	a = localtime (&wx);
	//uprintf("tmp: %d, time: %ld, %04d-%02d-%02d %02d:%02d\r\n", \
	//	atoi(str), wx, a->tm_year+1900, a->tm_mon+1, a->tm_mday, a->tm_hour, a->tm_min);
	sprintf(dest, "\\%04d%02d%02d", a->tm_year+1900, a->tm_mon+1, a->tm_mday);
}

int cari_files (char* pathxx, char *nf, int aksi) {
	char buf_lfn[255];
	FRESULT res;
	FILINFO fnoxx;
	DIR dirs;
	char *nama;
	int i=0;
	static char aaaa[64];
	static char bbbb[255];
	strcpy(aaaa, pathxx);
	unsigned int jum_dirs=0;
	//fileInfo.lfname = buf_lfn;
	//fileInfo.lfsize = 255;//sizeof (buf_lfn);
	static char lfnxx[_MAX_LFN * (_DF1S ? 2 : 1) + 1];
    fnoxx.lfname = lfnxx;
    fnoxx.lfsize = sizeof(lfnxx);
    
	if ((res = f_opendir (&dirs,  pathxx)))		{ 
		//uprintf("%s(): ERROR = %d\r\n", __FUNCTION__, res);
		return -1;
	}
	//printf("%s(): Open dir %s OK\r\n", __FUNCTION__, pathxx);
	
	char waktu[64];
	struct tm *a;
	unsigned int wx = (unsigned int) now_to_time(1, a);		// epoch
	a = localtime (&wx);
	sprintf(waktu, "%04d%02d%02d_%02d", a->tm_year+1900, a->tm_mon+1, a->tm_mday, a->tm_hour);
	//uprintf("=====> file waktu : %s\r\n", waktu);
	
	if (res == FR_OK) {
		for(;;) {
			res = f_readdir(&dirs, &fnoxx);
			
			if (res != FR_OK || fnoxx.fname[0] == 0) break;
			if (fnoxx.lfname[0] == 0)
				nama = &(fnoxx.fname [0]);
			else
				nama = &(fnoxx.lfname[0]);

			//sprintf(bbbb,"%s\\%s", aaaa, nama);		//
			//sprintf(bbbb,"%s\\",aaaa);
			strcpy(bbbb,aaaa);
			strcat(bbbb,"\\");
			strcat(bbbb,nama);
			//printf("path: %s, %s, nama: %s\r\n",aaaa, bbbb, nama);
			/*
			printf ("\r\n%c%c%c%c%c %u/%02u/%02u %02u:%02u %9u  %s",
					(fnoxx.fattrib & AM_DIR) ? 'D' : '-',
					(fnoxx.fattrib & AM_RDO) ? 'R' : '-',
					(fnoxx.fattrib & AM_HID) ? 'H' : '-',
					(fnoxx.fattrib & AM_SYS) ? 'S' : '-',
					(fnoxx.fattrib & AM_ARC) ? 'A' : '-',
					(fnoxx.fdate >> 9) + 1980, (fnoxx.fdate >> 5) & 15, fnoxx.fdate & 31,
					(fnoxx.ftime >> 11), (fnoxx.ftime >> 5) & 63,
					fnoxx.fsize, nama);
			//*/
			//*
			
			
			if (strncmp(waktu, nama, 11))	{		// 
				if (aksi == LIHAT)	{
					//uprintf("aksi: %d, path: %s, namafile: %s\r\n", aksi, bbbb, nama);
					i++;
				}
				if (aksi == LIHAT_ISI_SATU)	{
					//uprintf("aksi: %d, path: %s, namafile: %s\r\n", aksi, bbbb, nama);
					sprintf(pathxx, "%s", bbbb);
					sprintf(nf, "%s", nama);
					return 1;
				}
			}
			//*/
		}
	}
	return i;
}

int hapus_folder(char *fol)	{
	f_unlink(fol);
}

int cari_berkas(char *str_doku, char *path, int aksi) {
	char c = str_doku[0];
	if ((c!='H') && (c!='h') && (c!='J') && (c!='j') && (c!='B') && (c!='b') ) {
		printf("Argumen tidak benar !!\r\n");
		printf("Contoh : H-7, J-2, B-1\r\n");
		return -1;
	}
	
	char path_bk[127], namafile[64];
	char *pch, str[10], waktu[10];
	int i=0,j;
	strcpy(waktu,str_doku);
	pch=strstr(waktu,"-");
  	if (pch!=NULL)
  		strcpy(str, pch+1);

	//printf("str %s: %d\r\n", __FUNCTION__, atoi(str));
  	for(i=atoi(str); i>=0; i--) {
		sprintf(waktu, "%c-%d", waktu[0],i);
		cari_waktu(path_bk, waktu);
		j=cari_files(path_bk, namafile, aksi);
		//uprintf("_______________j:%d, waktu: %s, path: %s\r\n", j, waktu, path_bk);
		if (j==0)	hapus_folder(path_bk);
		if (aksi==LIHAT_ISI_SATU && j>0)	break;
		if (j==90)	break;
	}
	
	if (aksi==LIHAT_ISI_SATU)	{
		sprintf(path, "%s", path_bk);
	}
	
	return j;
	//*/
}

#endif
