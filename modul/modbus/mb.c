
#include "FreeRTOS.h"
#include "monita.h"
#include "hardware.h"
#include "ap_utils.h"
#include "mb.h"
#include <time.h>

#ifdef PAKAI_MODBUS

extern volatile float data_f[];
extern struct t_mavg *st_mavg;
extern char strmb[];
extern char outmb[];
extern char strmb3[];
extern char outmb3[];

#ifdef PAKAI_SERIAL_2
int cek_crc_mod(int nstr, unsigned char *x)	{
	unsigned char lo, hi;

	unsigned int i, Crc = 0xFFFF;
	for (i=0; i<(nstr-2); i++) {
		//Crc = CRC16 (Crc, x[i]);
		Crc = CRC16 (Crc, strmb[i]);
	}
	hi = ((Crc&0xFF00)>>8);	lo = (Crc&0xFF);
	//uprintf("CRC Modbus: %04X\r\n", Crc);
	
	//if (hi==x[nstr-1] && lo==x[nstr-2]) {
	if (hi==strmb[nstr-1] && lo==strmb[nstr-2]) {
		//printf("SIP crc\r\n");
		return 1;
	}
	return 0;
}
#endif

int get_crc_mod(int nstr, unsigned char *x)	{
	#if 0
	int k;
	printf("masuk %s\r\nCmd modbus: ", __FUNCTION__);
	#if 0
	for(k=0; k<6; k++)	{
		printf("%02X ", x[k]);
	}
	#endif
	printf("\r\n");
	#endif

	unsigned int i, Crc = 0xFFFF;
	for (i=0; i<nstr; i++) {
		Crc = CRC16 (Crc, x[i]);
	}
	#if 0
	//unsigned char lo, hi;
	//hi = ((Crc&0xFF00)>>8);	lo = (Crc&0xFF);
	//printf("hi: %02X, lo: %02X\r\n", hi, lo);
	#endif
	return Crc;
	
}

unsigned short update_bad_crc(unsigned short bad_crc, unsigned short ch) 	{ 
	const unsigned int Poly16=0x1021;
    unsigned short i, xor_flag;

    ch<<=8;
    for(i=0; i<8; i++) { 
		if ((bad_crc ^ ch) & 0x8000)	{ 
			xor_flag = 1; 
        } 
        else { 
			xor_flag = 0; 
        } 
        bad_crc = bad_crc << 1; 
        if (xor_flag)	{ 
            bad_crc = bad_crc ^ Poly16; 
        } 
        ch = ch << 1; 
    }
    return bad_crc;
}

unsigned short crc_ccitt_0xffff(int len, char *data)	{
	unsigned short bad_crc=0xFFFF; 
	unsigned char lo, hi;

	int i;
	for (i=0; i<len; i++) {
		bad_crc = update_bad_crc (bad_crc, data[i]);
		//printf(" %02x: %02x", data[i], bad_crc);
	}
	return bad_crc;
}

 #ifdef PAKAI_SERIAL_2
unsigned short cek_crc_ccitt_0xffff(int len, char *data)	{
	unsigned short bad_crc=0xFFFF; 
	unsigned char lo, hi;

	int i;
	for (i=0; i<(len-2); i++) {
		//bad_crc = update_bad_crc (bad_crc, data[i]);
		bad_crc = update_bad_crc (bad_crc, strmb[i]);
		//printf(" %02x: %02x", data[i], bad_crc);
	}
	//uprintf("\r\nCRC Modbus: %04X\r\n", bad_crc);
	hi = ((bad_crc&0xFF00)>>8);	lo = (bad_crc&0xFF);
	//uprintf("hi: %02x %02x --- lo: %02x %02x\r\n", hi, data[len-2], lo, data[len-1]);
	
	if (hi==strmb[len-2] && lo==strmb[len-1]) {
		//printf("SIP crc ccitt 0xffff\r\n");
		return 1;
	}
	return 0;
}
#endif

unsigned char simpan_mb_monita(int jml, unsigned char *s, int reg)	{
	int i, j, k, tmpFl=0, ff=0, no=0;
	float *fl;
	unsigned char nox;
	//int cnt;
	
	struct t_data *st_data;
	#if 0
	struct t_env *st_env;
	
	cnt = st_env->n_mavg;
	
	struct t_mavg *st_mavg;
	st_mavg = pvPortMalloc( cnt * sizeof (struct t_mavg) );
	if (st_mavg == NULL)	{
		printf(" %s(): ERR allok memory gagal !\r\n", __FUNCTION__);
		vPortFree (st_mavg);
		return 3;
	}
	#endif
	
	#ifdef ERROR_DATA_RATE
		if (olah == 0xFFFF) olah = 0;
	#endif
	
	//printf("jml: %d, reg: %d\r\n", jml, reg);
	jml = jml/4;
	for(i=0; i<jml; i++)	{
		for (k=0; k<JML_SUMBER; k++ ) 	{
			st_data = ALMT_DATA + k*JML_KOPI_TEMP;
			for (j=0; j<PER_SUMBER; j++)	{
				if ((reg+i)==st_data[j].id)	{
					no = k*PER_SUMBER+j;
					//printf("reg: %d, dataf %d\r\n", reg+i, (k*PER_SUMBER+j));
					ff=1;
					break;
				}
			}
			if (ff==1)	break;
		}
		ff = 0;
		
		//printf("%02x %02x %02x %02x : ", s[i*4+0], s[i*4+1], s[i*4+2], s[i*4+3]);
		tmpFl = ((s[i*4+0] & 0xFF)<<24) | ((s[i*4+1] & 0xFF)<<16) | ((s[i*4+2] & 0xFF)<<8) | (s[i*4+3] & 0xFF);
		fl = (float *)&tmpFl;
		
		if (st_data[i].mv_avg == 1)
			{
				nox = st_data[i].no_ma;
				st_mavg[nox].ke_0 = *fl;
			}
		else data_f[no] = *fl;
		//data_f[no] = *fl;

		//printf("dfloat: %08x %.3f\r\n", tmpFl, *fl);
	}
	//vPortFree(st_mavg);
	#ifdef ERROR_DATA_RATE
		olah ++;
		printf("s_m_m=%d\n\r", olah);
	#endif
	//printf("_____%s_____\r\n", __FUNCTION__);
	//printf("|mbus|\n\r");
}

unsigned char simpan_mb_gwr(int jml, unsigned char *s, int reg)	{
	int i, j, k, tmpFl=0, ff=0, no=0;
	float *fl;
	unsigned char nox;
	//int cnt;
	
	struct t_data *st_data;
	#if 0
	struct t_env *st_env;
	
	cnt = st_env->n_mavg;
	
	struct t_mavg *st_mavg;
	st_mavg = pvPortMalloc( cnt * sizeof (struct t_mavg) );
	if (st_mavg == NULL)	{
		printf(" %s(): ERR allok memory gagal !\r\n", __FUNCTION__);
		vPortFree (st_mavg);
		return 3;
	}
	#endif
	
	#ifdef ERROR_DATA_RATE
		if (olah == 0xFFFF) olah = 0;
	#endif
	
	//printf("jml: %d, reg: %d\r\n", jml, reg);
	jml = jml/4;
	for(i=0; i<jml; i++)	{
		for (k=0; k<JML_SUMBER; k++ ) 	{
			st_data = ALMT_DATA + k*JML_KOPI_TEMP;
			for (j=0; j<PER_SUMBER; j++)	{
				if ((reg+i)==st_data[j].id)	{
					no = k*PER_SUMBER+j;
					//printf("reg: %d, dataf %d\r\n", reg+i, (k*PER_SUMBER+j));
					ff=1;
					break;
				}
			}
			if (ff==1)	break;
		}
		ff = 0;
		
		//printf("%02x %02x %02x %02x : ", s[i*4+0], s[i*4+1], s[i*4+2], s[i*4+3]);
		tmpFl = ((s[i*4+0] & 0xFF)<<24) | ((s[i*4+1] & 0xFF)<<16) | ((s[i*4+2] & 0xFF)<<8) | (s[i*4+3] & 0xFF);
		fl = (float *)&tmpFl;
		
		if (st_data[i].mv_avg == 1)
			{
				nox = st_data[i].no_ma;
				st_mavg[nox].ke_0 = *fl;
			}
		else data_f[no] = *fl;
		//data_f[no] = *fl;

		//printf("dfloat: %08x %.3f\r\n", tmpFl, *fl);
	}
	//vPortFree(st_mavg);
	#ifdef ERROR_DATA_RATE
		olah ++;
		printf("s_m_g=%d\n\r", olah);
	#endif
	//printf("_____%s_____\r\n", __FUNCTION__);
	//printf("|mbus|\n\r");
}

unsigned char simpan_mb_std(int jml, unsigned char *s, int reg)	{
	int i, j, k, tmpFl=0, ff=0, no=0;
	float fl;
	unsigned char nox;
	//int cnt;
	
	struct t_data *st_data;
	#if 0
	struct t_env *st_env;
	
	cnt = st_env->n_mavg;
	
	struct t_mavg *st_mavg;
	st_mavg = pvPortMalloc( cnt * sizeof (struct t_mavg) );
	if (st_mavg == NULL)	{
		printf(" %s(): ERR allok memory gagal !\r\n", __FUNCTION__);
		vPortFree (st_mavg);
		return 3;
	}
	#endif
	
	#ifdef ERROR_DATA_RATE
		if (olah_std == 0xFFFF) olah_std = 0;
	#endif
	
	//printf("jml: %d, reg: %d\r\n", jml, reg);
	jml = jml/2;
	for(i=0; i<jml; i++)	{
		for (k=0; k<JML_SUMBER; k++ ) 	{
			st_data = ALMT_DATA + k*JML_KOPI_TEMP;
			for (j=0; j<PER_SUMBER; j++)	{
				if ((reg+i)==st_data[j].id)	{
					no = k*PER_SUMBER+j;
					//printf("reg: %d, dataf %d\r\n", reg+i, (k*PER_SUMBER+j));
					ff=1;
					break;
				}
			}
			if (ff==1)	break;
		}
		ff = 0;
		
		//printf("%02x %02x %02x %02x : ", s[i*4+0], s[i*4+1], s[i*4+2], s[i*4+3]);
		tmpFl = 0x0000<<16 | ((s[i*2+0] & 0xFF)<<8) | (s[i*2+1] & 0xFF);
		//printf("tmpfl=%x\n\r",tmpFl);
		//fl = (float *)&tmpFl;
		fl = (float) tmpFl;
		
		if (st_data[i].mv_avg == 1)
			{
				nox = st_data[i].no_ma;
				st_mavg[nox].ke_0 = fl;
			}
		else data_f[no] = fl;
		//data_f[no] = *fl;
		
		//printf("dfloat: %08x %.3f\r\n", tmpFl, *fl);
	}
	
	//vPortFree(st_mavg);
	#ifdef ERROR_DATA_RATE
		olah_std ++;
		printf("s_m_b=%d\n\r", olah_std);
	#endif
	//printf("_____%s_____\r\n", __FUNCTION__);
}

unsigned int CRC16(unsigned int crc, unsigned int data)		{
	const unsigned int Poly16=0xA001;
	unsigned int LSB, i;
	crc = ((crc^data) | 0xFF00) & (crc | 0x00FF);
	for (i=0; i<8; i++) {
		LSB = (crc & 0x0001);
		crc = crc/2;
		if (LSB)
			crc=crc^Poly16;
	}
	return(crc);
}

int kirim_respon_mb(int jml, char *s, int timeout, int serial)		{
	int i, k=0;
	//vTaskDelay(timeout/2);
	vTaskDelay(40);
	#ifdef PAKAI_SERIAL_2
	if (serial==2)	{
		enaTX2_485();
		for (i=0; i<jml; i++)	{
			k += xSerialPutChar2 (0, outmb[i], 10);
			
			#if 0
			//k++;
			printf("%02X", outmb[i]);
			#endif
		}
		#if 0
		printf("\n\r");
		#endif
		vTaskDelay(timeout);
		disTX2_485();
	}
	#endif
	
	#ifdef PAKAI_SERIAL_3
	if (serial==3)	{
		//printf("_____%s_____\r\n", __FUNCTION__);
		enaTX3_485();
		for (i=0; i<jml; i++)	{
			k += xSerialPutChar3 (0, s[i], 10);
			
			#if 0
			//k++;
			printf("%02X ", s[i]);
			#endif
		}
		//vSerialPutString3(1, "tes3\r\n", 6);
		vTaskDelay(timeout);
		disTX3_485();
	}
	#endif
	return k;
}

#ifdef PAKAI_SERIAL_2
int respon_modbus(int cmd, int reg, int jml, char *str, int len)	{
	//uprintf("-->%s, cmd: 0x%02x=%d, reg: %04x=%d, jml: %d\r\n\r\n", __FUNCTION__, cmd, cmd, reg, reg, jml);
	int i=0, j, index=0;
	char ketemu=0;
	
	if (cmd<READ_FILE_CONTENT)	{
		struct t_data *st_data;
		do	{
			st_data = ALMT_DATA + i*JML_KOPI_TEMP;
			//printf("i: %d, st_data: %08x\r\n", i, st_data);
			for (j=0; j<PER_SUMBER; j++)	{
			//	printf("id: %d, reg: %d\r\n", st_data[j].id, reg);
				if (st_data[j].id == reg)	{
			//		printf("ketemu: %d\r\n", i*PER_SUMBER+j);
					ketemu=1;
					index = i*PER_SUMBER+j;
					break;
				}
			}
			i++;
			if (i>JML_SUMBER)	{
				printf("===> ID tidak ditemukan !!\r\n");
				return 0;
			}
		} while (ketemu==0);
		//uprintf("Data index: %d\r\n", index);
	}
	
	if (cmd==READ_HOLDING_REG)		{
		return baca_reg_mb(index, jml);
	}
	if (cmd==WRITE_MULTIPLE_REG)	{
		//return tulis_reg_mb(reg, index, jml, str);
		return tulis_reg_mb(reg, index, jml, strmb);
	}
	if (cmd==READ_FILE_NAME)		{
		
	}
	
	if (cmd==READ_FILE_CONTENT)		{				// #define READ_FILE_CONTENT		25
		//uprintf("==> Modbus READ FILE COntent skywave\r\n");
		#ifdef PAKAI_FILE_SIMPAN
			//baca_kirim_file(reg, len, str);
			baca_kirim_file(reg, len, strmb);
		#endif
	}
	if (cmd==SENDED_FILE)		{				// #define READ_FILE_CONTENT		25
		//uprintf("==> hapus file FILE SENDED\r\n");
		#ifdef PAKAI_FILE_SIMPAN
			//int kk = proses_file_terkirim(len, str);
			int kk = proses_file_terkirim(len, strmb);
			uprintf(" hasil sended : %d\r\n", kk);
		#endif
	}
	if (cmd==KIRIM_IDMODEM)
	{
		baca_id_modem(strmb);
		//uprintf("|ID\n\r");
	}
	if (cmd==KIRIM_WAKTU)
	{
		baca_waktu_modem(strmb);
		//uprintf("|T\n\r");	
	}
	if (cmd==RESET)
	{
		baca_reset(strmb);
		//uprintf("|R\n\r");
	}
	return 10;
}
#endif

int baca_reset(char *str){
	char reset[5];
	char key[5] = "reset";
	int len;
	int i;
	
	len = (int) strmb[2];
	for (i=0; i<len; i++){	
		reset[i] = (char) strmb[3+i];	
		//uprintf("R[i]=%C\n\r",reset[i]);
	}	
	
	if (strcmp(key,reset) != 0) uprintf ("reset\n\r");
	else uprintf("no_reset\n\r");
}

int baca_waktu_modem(char *str){
	struct tm *wm;
	unsigned int wkt;
	struct t_env *st_env;
	st_env = pvPortMalloc( sizeof (struct t_env) );

	if (st_env==NULL)	{
		uprintf("  GAGAL alokmem !");
		vPortFree (st_env);
		return;
	}
	memcpy((char *) st_env, (char *) ALMT_ENV, (sizeof (struct t_env)));
	
	char t_modem[8];
	int len;
	int i;
	
	len = (int) strmb[2];
	for (i=0; i<len; i++){	
		t_modem[i] = (int) strmb[3+i];	
		//uprintf("T[i]=%C\n\r",t_modem[i]);
	}	
	//strcpy(st_env->waktu_modem, t_modem);
	st_env->waktu_modem = (int)strtol(t_modem, NULL, 16); //epoch
	wkt = st_env->waktu_modem;
	uprintf("waktu_modem=%d",st_env->waktu_modem);
	
	wm = localtime (&wkt);
	//uprintf(" |%04d|%02d|%02d|%02d|%02d|%02d|",wm->tm_year+1900, wm->tm_mon+1, wm->tm_mday, wm->tm_hour, wm->tm_min, wm->tm_sec);
	uprintf("\n\r");
	
	/* update waktu */	
	rtcWrite( wm );
	
	simpan_st_rom(SEKTOR_ENV, ENV, 0, (unsigned short *) st_env, 0);
	vPortFree (st_env);
}

int baca_id_modem(char *str) {
	struct t_env *st_env;
	st_env = pvPortMalloc( sizeof (struct t_env) );

	if (st_env==NULL)	{
		uprintf("  GAGAL alokmem !");
		vPortFree (st_env);
		return;
	}
	memcpy((char *) st_env, (char *) ALMT_ENV, (sizeof (struct t_env)));
	
	char ID[15];
	int len;
	int i;

	len = (int) strmb[2];

	for (i=0; i<len; i++){	
		ID[i] = (char) strmb[3+i];	
		//uprintf("ID[i]=%C\n\r",ID[i]);
	}	
	strcpy(st_env->id_modem, ID);
	//uprintf("id_modem=%s\n\r",st_env->id_modem);

	simpan_st_rom(SEKTOR_ENV, ENV, 0, (unsigned short *) st_env, 0);
	vPortFree (st_env);

}

#ifdef PAKAI_FILE_SIMPAN
int baca_kirim_file(int no, int len, char *str)		{
	char path[64], nf[32];
	unsigned long int size, i;
	int res, lenTot=0, lenPar=0, nFilemulai=0, ufile=0, nmx=0;
	FIL fd2;
	char *respon;
	FILINFO *finfo;
	
	if (no==0)	{
		//cari_berkas("H-2", LIHAT);
		//cari_berkas("H-3", path, LIHAT_ISI_SATU);
		cari_berkas(KIRIM_FILE_MULAI_WAKTU, path, LIHAT_ISI_SATU);
		//uprintf("no: %d ---> path: %s\r\n", no, path, strlen(nf));
		
		if (res = f_open(&fd2, path, FA_OPEN_EXISTING | FA_READ)) {
			if (res!=FR_NO_FILE)
				uprintf("%s(): Buka file error %d : %s !\r\n", __FUNCTION__, res, path);
			return 0;
		}
		
		lenPar = lenTot = fd2.fsize;
		if (lenTot>MAX_SEND_FILE_MB)	lenPar = MAX_SEND_FILE_MB;
		//uprintf("fsize: %d/%d\r\n", lenPar, fd2.fsize);
		
		nmx = fd2.fsize + 2 + 8 + 20 + 2;	// header + 2*file + namafile + crc
		#if 0
		respon = pvPortMalloc( nmx );		// nMallox
		if (respon == NULL)	{
			uprintf(" %s(): ERR allok memory gagal !\r\n", __FUNCTION__);
			f_close(&fd2);
			vPortFree (respon);
			return 0;
		}
		#endif
		strcpy(nf, pisah_nf(path));
		
		//f_read(&fd2, &respon[30], fd2.fsize, &ufile);
		f_read(&fd2, &outmb[30], fd2.fsize, &ufile);

		//uprintf("namafile: %s : %d\r\n", nf, ufile);
		#if 0
		
		int kk,ll, h=0;
		for (kk=0; kk<fd2.fsize; kk++)	{
			uprintf(" %02x", respon[30+kk]);
			h++;
			if (h>8)	uprintf("   ");
			if (h>16)	{ 	h=0; uprintf("\r\n");	}
		}
		#endif
	} 
	else {
		nFilemulai = MAX_SEND_FILE_MB*(no+1);
		lenPar = MAX_SEND_FILE_MB*no;
		lenTot = fd2.fsize;
	}
	
	//f_lseek( &fd2, finfo);
	//uprintf("fsize: %d\r\n", finfo->fsize);
	//f_read( &fd2, path, 6, &res);
	//f_read( &fd, buffer, ret, &ln);
	
	// 2  : header : IDslave + CMD
	// 8  : [4]+[4] pjg file sub + tot
	// 20 : nama file
	
	outmb[0] = str[0];		outmb[1] = str[1];
	memcpy(&outmb[2], (void*) &lenPar, 4);
	memcpy(&outmb[6], (void*) &lenTot, 4);
	memcpy(&outmb[10], (void*) &nf, strlen(nf));	outmb[10+strlen(nf)]  = '\0';
	
	#if 0
	nmx = 8;
	outmb[0] = 0x11;	outmb[1] = 0x25;	
	outmb[2] = 0x00;	outmb[3] = 0x00;
	outmb[4] = 0x00;	outmb[5] = 0x00;
	#endif
	
	unsigned short bad_crc=crc_ccitt_0xffff(nmx-2, outmb);
	outmb[nmx-2] = ((bad_crc&0xFF00)>>8);
	outmb[nmx-1] = (bad_crc&0xFF);

	#if 0
		uprintf("namafile: %s : %d\r\n", nf, ufile);
		int kk,ll, h=0;
		for (kk=0; kk<nmx; kk++)	{
			uprintf(" %02x", outmb[kk]);
			h++;
			if (h==8)	uprintf("   ");
			if (h==16)	{ 	h=0; uprintf("\r\n");	}
		}
	#endif
	
	kirim_respon_mb(nmx, outmb, 3000, 2);
	vTaskDelay(100);
	f_close(&fd2);
	//hapus_folder_kosong();
	
	//vPortFree (respon);
}

int proses_file_terkirim(int len, char *str)	{
	char nf[32], path[64], pch[64];
	int x = (int) (str[2]<<8 | str[3]);
	memcpy(nf, &str[4], x);
	uprintf("nama file dikirim: %s\r\n", nf);
	
	FIL fd2;
	FRESULT res;
	DIR dir;

	strncpy(pch, nf, 8); pch[8] = 0;
	sprintf(path, "\\%s\\%s", pch, nf);
	
	res = f_opendir(&dir, FOLDER_SENDED);		// masuk ke folder \\SENDED\\ //
	uprintf("buka folder %s: %d, %d\r\n", FOLDER_SENDED, res );
	if (res != FR_OK)	{
		f_opendir(&dir, "\\");		// masuk ke folder \\SENDED\\ //
		res = f_mkdir(FOLDER_SENDED);
		//if (res != FR_OK)	return 1;
		uprintf("BUKA file %s GAGAL, bikin dulu !!\r\n", FOLDER_SENDED);
		res = f_opendir(&dir, FOLDER_SENDED);
		if (res != FR_OK)	return 2;
		
		#if 0
		res = f_opendir(&dir, pch);
		if (res != FR_OK)	{
			res = f_mkdir(pch);
			if (res != FR_OK)	return 3;
			res = f_opendir(&dir, pch);
			if (res != FR_OK)	return 4;
		}
		#endif
	}
	
	sprintf(pch, "%s\\%s", FOLDER_SENDED, nf);
	//uprintf("path: %s, ke: %s\r\n", path, pch);
	res = f_rename(path, pch);
	uprintf(" File %s sudah terkirim & dipindah ke %s: %d\r\n", nf, pch, res);
	
	int kk=1;
	while(res == 8) 	{
		sprintf(pch, "\\%s\\dob%d_%s", FOLDER_SENDED, kk, nf);
		//uprintf("file %s dobel ke %s !!!\r\n", path, pch);
		res = f_rename(path, pch);
		uprintf(" >>>>> File DOBEL %s sudah terkirim & dipindah ke %s: %d\r\n", nf, pch, res);
		kk++;
		//vTaskDelay(1000);
		//res = 0;
	}
	//f_chdir("\\");
	
	return res;
}

#endif

int baca_reg_mb(int index, int jml)	{			// READ_HOLDING_REG
	int i, nX, j=0, njml=0;
	char *respon; 
	
	//njml = (int) (jml/2);
	njml = (int) (jml);
	nX = jml_st_mb3H(njml);
	
	#if 0
	respon = pvPortMalloc( nX );
	
	if (respon == NULL)	{
		printf(" %s(): ERR allok memory gagal !\r\n", __FUNCTION__);
		vPortFree (respon);
		return 0;
	}
	#endif
	//printf("Alok: %d @%#08x\r\n", nX, respon);
	
	struct t_env *st_env;
	st_env = ALMT_ENV;
	
	outmb[0] = st_env->almtSlave;
	outmb[1] = READ_HOLDING_REG;
	outmb[2] = njml*4;
	//outmb[2] = njml;
	
	unsigned int *ifl;
	for (i=0; i<njml; i++)	{
		if (1)	{		// almt ROM lintas struct (kelipatan 10)
			
		}
		
		ifl = (unsigned int *) &data_f[index+i];
		//printf("  data[%d]: %.2f = 0x%08x\r\n", index+i, data_f[index+i], *ifl);
		outmb[3+i*4] = (unsigned char) (*ifl>>24) & 0xff;
		outmb[4+i*4] = (unsigned char) (*ifl>>16) & 0xff;
		outmb[5+i*4] = (unsigned char) (*ifl>> 8) & 0xff;
		outmb[6+i*4] = (unsigned char) (*ifl & 0xff);
	}


	unsigned int Crc = 0xFFFF;
	for (i=0; i<(nX-2); i++)	{
		Crc = CRC16 (Crc, outmb[i]);
	}
	outmb[nX-1] = ((Crc&0xFF00)>>8);	
	outmb[nX-2] = (Crc&0xFF);
	
	#if 0
	printf("===> Respon[%d]: ", nX);
	for (i=0; i<nX; i++)		{
		printf(" %02x", respon[i]);
		// kirim Serial2 modbus
		//xSerialPutChar2 (0, respon[i], 0xffff);
	}
	printf("\r\n\r\n");
	#endif
	
	
	kirim_respon_mb(nX, outmb, 100, 2);
	
	#if 0
	enaTX2_485();
	vSerialPutString2(0, respon, nX);
	vTaskDelay(100);
	disTX2_485();
	#endif
	
	//vPortFree (respon);
	return nX;
}

int tulis_reg_mb(int reg, int index, int jml, char* str)	{	// WRITE_MULTIPLE_REG: 16
	int i, j, tmpFl, njml;
	char *respon; 
	
	//************  PROSESS  ************//
	// 32bit
	njml = (int) (jml/2);
	int nby, byte = str[6];
	if (byte%4!=0)	{
		printf("===> Data 32bit tidak VALID\r\n");
		return 1;
	}
	
	float *fl;
	struct tm aaa;
	unsigned int wx = (unsigned int) now_to_time(1,aaa);		// epoch
	for (i=0; i<njml; i++)	{
		tmpFl = (str[7+i*4]<<24) | (str[8+i*4]<<16) | (str[9+i*4]<<8) | (str[10+i*4]) ;
		fl = (float *)&tmpFl;
		//uprintf("data[%d]: %.3f, 0x%08x\r\n", index+i, *fl, tmpFl);

		data_f[index+i] = *fl;
		if (index+i==24)	{			// waktu epoch !!!
			unsigned int wm = (unsigned int) data_f[index+i] + (60*60*7);
			
			//uprintf("epoch : %ld\r\n", wx);
		
			//uprintf(">>>>> sync waktu modem[%d] : %.0f\r\n", index+i, data_f[index+i]);
			#if 0
			if (st_hw.uuwaktu>2)	
			{
				uprintf("+++++ update waktu modem[%d] : %.0f, %ld, epoch monita: %ld !!!\r\n", index+i, data_f[index+i], wm, wx);
				sync_waktu_modem(wm);
				st_hw.uuwaktu=0;
			}
			#endif
		}
	}
	
	
	//************   BALAS   ************//
	#if 0
	respon = pvPortMalloc( jml_st_mb10H );
	
	if (respon == NULL)	{
		printf(" %s(): ERR allok memory gagal !\r\n", __FUNCTION__);
		vPortFree (respon);
		return 1;
	}
	//printf("Alok: %d @%#08x\r\n", jml_st_mb3H(jml), respon);
	#endif

	struct t_env *st_env;
	st_env = ALMT_ENV;
	
	outmb[0] = st_env->almtSlave;
	outmb[1] = WRITE_MULTIPLE_REG;
	outmb[2] = (reg>>8) & 0xff;
	outmb[3] = (reg & 0xff);
	outmb[4] = (jml>>8) & 0xff;
	outmb[5] = (jml & 0xff);
	
	unsigned int Crc = 0xFFFF;
	for (i=0; i<(jml_st_mb10H-2); i++)	{
		Crc = CRC16 (Crc, outmb[i]);
	}
	outmb[jml_st_mb10H-1] = ((Crc&0xFF00)>>8);	
	outmb[jml_st_mb10H-2] = (Crc&0xFF);
	
	#if 0
	printf("===> Respon: ");
	for (i=0; i<jml_st_mb10H; i++)		{
		printf(" %02x", outmb[i]);
		// kirim Serial2 modbus
		//xSerialPutChar2 (0, respon[i], 0xffff);
	}
	printf("\r\n\r\n");
	#endif
	
	kirim_respon_mb(jml_st_mb10H, outmb, 100, 2);
	
	#if 0
	enaTX2_485();
	//vSerialPutString2(0, respon, jml_st_mb10H);
	for (i=0; i<jml_st_mb10H; i++)		{
		xSerialPutChar2 (0, respon[i], 10);
	}
	vTaskDelay(50);
	disTX2_485();
	#endif
	
	//vPortFree (respon);
	return jml_st_mb10H;
}

int parsing_mb_native_cmd(char*s, char* cmd, int* dest)	{
	int i, n, k;
	char *p;	p = (void*) s;
	for (i=0; p[i]; p[i]==';' ? i++ : *p++);
	if (strlen(s)>0)	i++;
	//printf("p: %d. jml space: %d\r\n", strlen(s), i);
	
	int *buf;
	//buf = (int*) malloc(i*sizeof(int));
	buf = pvPortMalloc( i * sizeof (int) );
	
	if (buf == NULL)	{
		printf(" %s(): ERR allok memory gagal !\r\n", __FUNCTION__);
		vPortFree(buf);
		return -1;
	}
	
	p=(void*) s;	k=0;
	do {
		if (k==0)	buf[k] = atoi(s);
		else 		buf[k] = atoi(s+1);
		s=strchr(s+1,';');
		k++;
	} while(s!=NULL);
	
	#if 0
	for (k=0; k<i; k++)	{
		printf("isi buf[%d]: %d\r\n", k, buf[k]);
	}
	#endif
	
	*dest = buf[6];
	//printf("dest: %d, buf[6]: %d\r\n", *dest, buf[6]);
	
	unsigned int tmp;
	//*
	cmd[0] = buf[1];
	cmd[1] = buf[4]?WRITE_MULTIPLE_REG:READ_HOLDING_REG;
	
	//tmp = buf[2]>40000?(buf[2]-40000-buf[3]):buf[2];
	tmp = buf[2]>40000?(buf[2]-40001-buf[3]):(buf[2]-1);
	cmd[2] = (tmp >> 8) & 0xFF;
	cmd[3] = tmp & 0xFF;
	
	//tmp = buf[5]*2;
	tmp = buf[5];
	cmd[4] = (tmp >> 8) & 0xFF;
	cmd[5] = tmp & 0xFF;
	
	#if 0
	for (k=0; k<6; k++)	{
		printf("%02X", cmd[k]);
	}
	#endif
	
	int crc = get_crc_mod(6,cmd);
	cmd[6] = crc & 0xFF;
	cmd[7] = (crc >> 8) & 0xFF;
	
	//printf("crc: %04X\r\n",crc);
	//*/
	/*
	outmb3[0] = buf[1];
	outmb3[1] = buf[4]?WRITE_MULTIPLE_REG:READ_HOLDING_REG;
	
	tmp = buf[2]>40000?(buf[2]-40000-buf[3]):buf[2];
	outmb3[2] = (tmp >> 8) & 0xFF;
	outmb3[3] = tmp & 0xFF;
	outmb3[4] = (buf[5] >> 8) & 0xFF;
	outmb3[5] = buf[5] & 0xFF;
	
	#if 0
	for (k=0; k<6; k++)	{
		printf("%02X", cmd[k]);
	}
	#endif
	
	int crc = get_crc_mod(6,outmb3);
	outmb3[6] = (crc >> 8) & 0xFF;
	outmb3[7] = crc & 0xFF;
	//*/
	//printf("  %s(): Mallok @ %X, data[21]: %.2f, d[22]: %.0f\r\n", __FUNCTION__, buf, data_f[20], data_f[21]);
	
	vPortFree(buf);
	
	//cmd = mbcmd;
	#if 0
	printf("Cmd modbus: ");
	for(k=0; k<8; k++)	{
		printf("%02X ", cmd[k]);
	}
	printf("\r\n");
	#endif
	return 1;
}

#endif
