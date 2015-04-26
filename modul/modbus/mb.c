
#include "FreeRTOS.h"
#include "monita.h"
#include "hardware.h"
#include "ap_utils.h"
#include "mb.h"

#ifdef PAKAI_MODBUS

extern volatile float data_f[];
extern char strmb[];
extern char outmb[];

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

int kirim_respon_mb(int jml, char *s, int timeout)		{
	int i, k=0;
	vTaskDelay(40);
	enaTX2_485();
	for (i=0; i<jml; i++)	{
		k += xSerialPutChar2 (0, outmb[i], 10);
	}
	vTaskDelay(timeout);
	disTX2_485();
	return k;
}

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
		uprintf("==> Modbus READ FILE COntent skywave\r\n");
		#ifdef PAKAI_FILE_SIMPAN
			//baca_kirim_file(reg, len, str);
			baca_kirim_file(reg, len, strmb);
		#endif
	}
	if (cmd==SENDED_FILE)		{				// #define READ_FILE_CONTENT		25
		uprintf("==> hapus file FILE SENDED\r\n");
		#ifdef PAKAI_FILE_SIMPAN
			//int kk = proses_file_terkirim(len, str);
			int kk = proses_file_terkirim(len, strmb);
			uprintf(" hasil sended : %d\r\n", kk);
		#endif
	}
	return 10;
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

		uprintf("namafile: %s : %d\r\n", nf, ufile);
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
	
	kirim_respon_mb(nmx, outmb, 3000);
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
	
	njml = (int) (jml/2);
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
	
	
	kirim_respon_mb(nX, outmb, 100);
	
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
	
	kirim_respon_mb(jml_st_mb10H, outmb, 100);
	
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


#endif
