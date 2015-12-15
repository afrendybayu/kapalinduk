
#include "FreeRTOS.h"
#include "task.h"
#include "iap.h"
#include "monita.h"

#include <stdio.h>
#include <stdarg.h>

//#define DEBUG_IAP
extern volatile float data_f[];
extern volatile float astm_f[];

unsigned int param_table[5];

//typedef void (*IAP)(unsigned int [],unsigned int[]);

void iap_entry(unsigned int param_tab[], unsigned int result_tab[])		{
	IAP_return_t iap_return;
	void (*iap)(unsigned int[], unsigned int[]);
	
	iap = (void (*)(unsigned int[], unsigned int[]))IAP_ADDRESS;

	taskENTER_CRITICAL();
	iap(param_tab,result_tab);
	taskEXIT_CRITICAL();
}

IAP_return_t iapReadSerialNumber(void)	{
	IAP_return_t iap_return;
	// ToDo: Why does IAP sometime cause the application to halt when read???
	param_table[0] = IAP_CMD_READPARTID;

	iap_entry(param_table,(unsigned int*)(&iap_return));
	
	return iap_return;
}

IAP_return_t iapReadBootVersion(void)	{
	IAP_return_t iap_return;
	// ToDo: Why does IAP sometime cause the application to halt when read???
	param_table[0] = IAP_CMD_READBOOTCODEVERSION;
	
	iap_entry(param_table,(unsigned int*)(&iap_return));
	
	return iap_return;
}

IAP_return_t iapReadBlankSector(uchr awal, uchr akhir)		{
	IAP_return_t iap_return;
	// ToDo: Why does IAP sometime cause the application to halt when read???
	param_table[0] = IAP_CMD_BLANKCHECKSECTOR;
	param_table[1] = awal;
	param_table[2] = akhir;
	
	iap_entry(param_table,(unsigned int*)(&iap_return));
	
	return iap_return;
}

IAP_return_t iapPrepareSector(uchr awal, uchr akhir)		{
	IAP_return_t iap_return;
	// ToDo: Why does IAP sometime cause the application to halt when read???
	param_table[0] = IAP_CMD_PREPARESECTORFORWRITE;
	param_table[1] = awal;
	param_table[2] = akhir;
	
	iap_entry(param_table,(unsigned int*)(&iap_return));
	
	return iap_return;
}

IAP_return_t iapJob(uchr iapcmd, uchr awal, uchr akhir)		{
	IAP_return_t iap_return;
	// ToDo: Why does IAP sometime cause the application to halt when read???
	param_table[0] = iapcmd;
	param_table[1] = awal;
	param_table[2] = akhir;

	iap_entry(param_table,(unsigned int*)(&iap_return));
	
	return iap_return;
}

IAP_return_t iapEraseSector(uchr awal, uchr akhir)		{
	IAP_return_t iap_return;
	// ToDo: Why does IAP sometime cause the application to halt when read???
	param_table[0] = IAP_CMD_ERASESECTORS;
	param_table[1] = awal;
	param_table[2] = akhir;
	param_table[3] = (configCPU_CLOCK_HZ/configTICK_RATE_HZ);
	
	iap_entry(param_table,(unsigned int*)(&iap_return));
	
	return iap_return;
}

IAP_return_t iapCopyMemorySector(unsigned int addr, unsigned short * data, int pjg)		{
	IAP_return_t iap_return;
	// ToDo: Why does IAP sometime cause the application to halt when read???
	
	//printf("\r\n%s cmd: %d, addr: 0x%08X, pjg: %d\r\n", __FUNCTION__, IAP_CMD_COPYRAMTOFLASH, addr, pjg);
	
	param_table[0] = IAP_CMD_COPYRAMTOFLASH;
	param_table[1] = addr;
	param_table[2] = (unsigned short*) &data[0];
	param_table[3] = pjg;
	param_table[4] = (configCPU_CLOCK_HZ/configTICK_RATE_HZ);
	
	iap_entry(param_table,(unsigned int*)(&iap_return));
	
	return iap_return;
}

char hapuskan_sektor(int sektor)	{
	//int nSktr = cek_nomor_valid(sektor, JML_SECTOR_LPC-1);
	int nSktr = sektor;
	IAP_return_t iap_return = iapSiapSektor(nSktr, nSktr);
	
	if ( iap_return.ReturnCode == CMD_SUCCESS )	{
		#ifdef DEBUG_IAP
		printf("  SS[%d]", nSktr);
		#endif
		iap_return = iapEraseSector(nSktr, nSktr);
	} 
	else	{
		return 1;
	}

	if (iap_return.ReturnCode == CMD_SUCCESS)	{
		#ifdef DEBUG_IAP
		printf(" HS[%d].\r\n", nSktr);
		#endif
	}
	else	{
		#ifdef DEBUG_IAP
		printf("  GAGAL Hapus Sektor %d !!\r\n", nSktr);
		#endif
		return 2;
	}
	return 0;
}

int hitung_ram(int jml)	{
	int nn;
	
	if (jml > 0)    nn =  256;
	if (jml > 256)  nn =  512;
	if (jml > 512)  nn = 1024;
	if (jml > 1024) nn = 4096;
	
	return nn;
}

char simpan_data_rom(int no, ...)	{
	int i, nn, sektor, addr, hapus;
	unsigned short *pdata;
	
	va_list vl;
	va_start(vl,no);
	//printf("jml no: %d\r\n", no);

	for (i=0; i<no; i++)	{
		if (i==0)	{
			sektor = va_arg(vl, int);			// sektor
			//printf("%d Sektor: %d\r\n", i, sektor);
		}
		else if (i==1)	{					// alamat memori
			hapus = va_arg(vl, int);
			if (hapus)	hapuskan_sektor(sektor);
			//printf("%d Hapus: %d\r\n", i, hapus);
		}
		else if (i%3==2)	{					// alamat memori
			addr = va_arg(vl, int);
			//printf("%d addr: %d\r\n", i, addr);
		}
		else if (i%3==0)	{					// jml data
			nn = va_arg(vl, int);
			//printf("%d jml: %d\r\n", i, nn);
		} else {								// datanya
			pdata = va_arg(vl, char*);
			//printf("%d data: %s\r\n", i, pch);
			printf("--->sktr: %d, addr: 0x%08X, n: %d/%d\r\n", sektor, addr, nn, hitung_ram(nn));
			simpan_rom(sektor, addr,  (unsigned short *)pdata, hitung_ram(nn));
		}
	}
	va_end(vl);	
}

int ukuran_rom (char no)	{
	if (no>=0 && no<=JML_SECTOR_LPC-1)	{
		if (no>7 && no<22)		return	32;
		else 					return  4;
	} else {
		return 0;
	}
}

unsigned int alamat_sektor(int sektor)	{
	unsigned int alm = 0;
	int i;
	
	if (sektor>=0 && sektor<JML_SECTOR_LPC)		{		
		for (i=0; i<sektor; i++)	{
			if (i<8)			alm += 0x1000;
			else if (i<22)		alm += 0x8000;
			else 				alm += 0x1000;
		}
		return alm;
	}
	return 0;
}

char kopikan_sektor_tmp(int ALAMAT_SEKTOR)	{
	int i, almt_data, almt_tuj, hasil;
	char *pch, *pdata;
	
	hapuskan_sektor(SEKTOR_TEMP);
	
	// pindah tiap 1KB * 32
	for (i=0; i<SUMBER_PER_SEKTOR; i++)		{
		almt_data = i*JML_KOPI_TEMP + ALAMAT_SEKTOR;
		pch = (char *) almt_data;
		
		almt_tuj = i*JML_KOPI_TEMP + ALMT_SKTR_TEMP;
		pdata = pvPortMalloc(JML_KOPI_TEMP);
		if (pch!=NULL)	{
			taskENTER_CRITICAL();
			memcpy((char *) pdata, (char *) pch, JML_KOPI_TEMP);
			taskEXIT_CRITICAL();
		} else {
			printf("  GAGAL alokmem !\r\n");
			vPortFree (pdata);
			return;
		}
	
		hasil = simpan_rom(SEKTOR_TEMP, almt_tuj,  (unsigned short *)pdata, JML_KOPI_TEMP);
		vPortFree (pdata);
		if (hasil>0) return 1;
	}
	
	#if 0
	char *en;
	en = (char *) ALMT_SKTR_TEMP;
	for (i=0; i<16; i++)
		printf(" %02X", *(en++));
	printf("\r\n");
	
	en = (char *) ALAMAT_SEKTOR;
	for (i=0; i<16; i++)
		printf(" %02X", *(en++));
	printf("\r\n");
	#endif
	
	return 0;
}

char simpan_rom(int sektor, unsigned int addr, unsigned short *data, int jml)	{
	//if (hapuskan_sektor(sektor)==CMD_SUCCESS)	
	//int nSktr = cek_nomor_valid(sektor, JML_SECTOR_LPC-1);
	int nSktr = sektor;
	IAP_return_t iap_return = iapSiapSektor(nSktr, nSktr);
	//printf("  ---> hasil siap: %d\r\n", iap_return.ReturnCode);
	if ( iap_return.ReturnCode == CMD_SUCCESS )		{
		#ifdef DEBUG_IAP
		printf("  SS[%d]", nSktr);
		#endif
		
		iap_return = iapCopyMemorySector(addr, (unsigned short *) data, jml);
		//printf("  ---> hasil kopi: %d ", iap_return.ReturnCode);
	} 
	else
		return 1;
	
	if (iap_return.ReturnCode == CMD_SUCCESS)	{
		#ifdef DEBUG_IAP
		printf(" TS[%d].\r\n", nSktr);
		#endif
	}
	else	{
		#ifdef DEBUG_IAP
		printf(" GAGAL SALIN S%d : %d\r\n", nSktr, iap_return.ReturnCode);
		#endif
		return 2;
	}
	
	return 0;
}

#if 0
char simpan_struct_block_rom(int sektor, int st, int flag, char *pdata)	{
	char *pdata1, *pdata2;
	char *pch1, *pch2;
	int hapus=0, i;
	int jml, jml1, jml2;
	unsigned almt;
	
	if (sektor == SEKTOR_ENV)		{
		//printf("sektor: %d, st: %d\r\n", sektor, st);
		jml = 2 + 3*1;
		jml1 = cek_jml_struct(ENV);
		jml2 = cek_jml_struct(SUMBER)*JML_SUMBER;

		// ENV diamankan
		printf("env");
		//return 0;
		pdata1 = pvPortMalloc(jml1);
		printf("  %s(): Mallok @ %X\r\n", __FUNCTION__, pdata1);
		if (pdata1!=NULL)	{
			taskENTER_CRITICAL();
			if (st == ENV)	{
				memcpy((char *) pdata1, (char *) pdata, jml1);
			} else {
				memcpy((char *) pdata1, (char *) ALMT_ENV, jml1);
			}
			taskEXIT_CRITICAL();
			printf("  memSIP : %d, Alm: 0x%08X.", jml1, ALMT_ENV);
		} else {
			printf("  GAGAL alokmem !");
			//vPortFree (pdata1);
			return 1;
		}
		printf(" end\r\n");
		#if 0
		// SUMBER
		printf("sumber");
		pdata2 = pvPortMalloc(jml2);
		if (pdata2!=NULL)	{
			taskENTER_CRITICAL();
			if (st == SUMBER) 	{
				memcpy((char *) pdata2, (char *) pdata, jml2);
			} else {
				memcpy((char *) pdata2, (char *) ALMT_SUMBER, jml2);
			}
			taskEXIT_CRITICAL();
			printf("  memSIP : %d, Alm: 0x%08X.\r\n", jml2, ALMT_SUMBER);
		} else {
			printf("  GAGAL alokmem !\r\n");
			vPortFree (pdata1);
			vPortFree (pdata2);
			return 3;
		}
		printf(" end\r\n");
		#endif
#if 1
		hapuskan_sektor(SEKTOR_ENV);
		//simpan_data_rom(jml, SEKTOR_ENV, hapus, alm1, jml1, (unsigned short *)pch1, alm2, jml2, (unsigned short *)pch2);
		simpan_data_rom(jml, SEKTOR_ENV, hapus, ALMT_ENV,    jml1, (unsigned short *)pdata1);
		//simpan_data_rom(jml, SEKTOR_ENV, hapus, ALMT_SUMBER, jml2, (unsigned short *)pdata2);
#endif
		vPortFree (pdata1);
		//vPortFree (pdata2);
	}
	else if (sektor == SEKTOR_DATA)	{				// 10 data, per_sumber
		jml1 = cek_jml_struct(DATA)*PER_SUMBER;
		jml = 2 + 3*1;
		IAP_return_t iap_return = iapReadBlankSector(SEKTOR_DATA, SEKTOR_DATA);
		if ( (iap_return.ReturnCode==SECTOR_NOT_BLANK) && (st!=-1) )
			kopikan_sektor_tmp(SEKTOR_DATA);
		hapuskan_sektor(SEKTOR_DATA);
		
		for (i=0; i<JML_SUMBER; i++)	{
		//for (i=0; i<SUMBER_PER_SEKTOR; i++)	{
			almt = i*JML_KOPI_TEMP + ALMT_DATA;
			pdata1 = pvPortMalloc(JML_KOPI_TEMP);
			if ( (st==i) || (st==-1) )	{				// data yg berubah
				printf("-1/%d almt: 0x%08X\r\n", i, almt);
				if (pdata1!=NULL)	{
					taskENTER_CRITICAL();
					memcpy(pdata1, pdata, jml1);
					taskEXIT_CRITICAL();
					//printf("  memSIP : %d, Alm: 0x%08X.\r\n", jml2, alm2);
				} else {
					printf("  GAGAL alokmem: %d !\r\n", i+1);
					vPortFree (pdata1);
					return 5;
				}
			} else {
				if (pdata1!=NULL)	{
					taskENTER_CRITICAL();
					memcpy(pdata1, (char *) ALMT_DATA, jml1);
					taskEXIT_CRITICAL();
					//printf("  memSIP : %d, Alm: 0x%08X.\r\n", jml2, alm2);
				} else {
					printf("  GAGAL alokmem: %d !\r\n", i+1);
					vPortFree (pdata1);
					return 6;
				}
			}
			//simpan_data_rom(jml, SEKTOR_DATA, hapus, almt, JML_KOPI_TEMP, (unsigned short *)pdata1);
			simpan_rom(SEKTOR_DATA, almt,  (unsigned short *)pdata1, JML_KOPI_TEMP);
			vPortFree (pdata1);
		}
	}
	vPortFree (pdata1);
	vPortFree (pdata2);
	return 0;
}
#endif

void generate_data_random()		{
	int i;
	for (i=0; i<JML_SUMBER*PER_SUMBER; i++)		{
		data_f[i] = ((float)rand() / 1000000);
	}
}

void load_data_rtc()	{
	int i;
	char status;
	float kf;
	
	IAP_return_t iap_return = iapReadBlankSector(SEKTOR_ENV, SEKTOR_ENV);
	
	if (iap_return.ReturnCode == SECTOR_NOT_BLANK)	{		// setting sudah ada
		uprintf("  Baca data dari memRTC\r\n");
		struct t_env *st_env;
		st_env = ALMT_ENV;
		
		for (i=0; i<JML_KANAL; i++)		{
			status = st_env->kalib[i].status;
			data_f[i] = kf = *( (float*) &(*(&MEM_RTC0+(RTC_MEM_START+i+1))));
			if (status==sRUNNING_HOURS)		{
				konter.t_konter[i].rh_x = (int ) data_f[i];		// total sebelumnya
			}
			if (status==sONOFF_RH)		{
				konter.t_konter[i].rh_x = (int ) data_f[i];		// total sebelumnya
				//uprintf("RUNHOUR[%d] : %d  --> ", i+1, konter.t_konter[i].rh_x);
			}
			if (status==sRPM_RH)		{
				konter.t_konter[i].rh_x = (int ) data_f[i];		// total sebelumnya
				//uprintf("RUNHOUR[%d] : %d  --> ", i+1, konter.t_konter[i].rh_x);
			}
			if (status==sFLOWx)		{
				konter.t_konter[i].hit = (int) ( (kf-st_env->kalib[i].C)/st_env->kalib[i].m );
				d_t1[i] = data_f[i];
				//uprintf("HIT[%d] : %d  --> ", i+1, konter.t_konter[i].hit);
			}
			//uprintf("i: %d --> dataf[%d]: %.3f\r\n", i, i, data_f[i]);
			//uprintf("data[%2d]: %.2f\r\n", i, data_f[i]);
		}
		for (i=0; i<JML_KANAL; i++)		{
			astm_f[i] = kf = *( (float*) &(*(&MEM_RTC0+(RTC_MEM_START+i+71))));
		}
	}
	else if (iap_return.ReturnCode == CMD_SUCCESS)	{		// setting KOSONG
	
	}
	
	
	
}

void baca_konfig_rom()		{
	//printf("Data env & sumber\r\n");
	IAP_return_t iap_return = iapReadBlankSector(SEKTOR_ENV, SEKTOR_ENV);
	int i;
	
	if (iap_return.ReturnCode == SECTOR_NOT_BLANK)	{		// setting sudah ada
		printf("Baca Konfig ENV & SUMBER  ");
		//load_data_rtc();
	}
	else if (iap_return.ReturnCode == CMD_SUCCESS)	{		// setting KOSONG
		printf(">> Init ROM: ENV, SUMBER\r\n");
		set_env_default();
		set_sumber_default();
		
		
		#ifdef PAKAI_FILE_SIMPAN
		printf(">> Init ROM: FILE\r\n");
		set_file_default();
		#endif
	} else {
		
	}
	//printf("Data data\r\n");
	//generate_data_random();
	iap_return = iapReadBlankSector(SEKTOR_DATA, SEKTOR_DATA);
	if (iap_return.ReturnCode == SECTOR_NOT_BLANK)	{		// setting sudah ada
		printf("Baca Konfig DATA");
	}
	else if (iap_return.ReturnCode == CMD_SUCCESS)	{		// setting KOSONG
		printf(">> Init ROM: DATA\r\n");
		set_data_default();
	}
	printf("\r\n");
}

char kopi_semua_blok(int almt)		{
	
	if (almt != SUMBER)	{
		struct t_sumber *st_sumber;
		st_sumber = pvPortMalloc( sizeof (struct t_sumber)*JML_SUMBER );
				
		if (st_sumber==NULL)	{
			printf("  GAGAL alokmem SUMBER !");
			vPortFree (st_sumber);
			return 1;
		}
		//printf("  %s(): Mallok @ %X\r\n", __FUNCTION__, st_sumber);
		taskENTER_CRITICAL();
		memcpy((char *) st_sumber, (char *) ALMT_SUMBER_TMP, sizeof (struct t_sumber)*JML_SUMBER);
		taskEXIT_CRITICAL();
		simpan_rom(SEKTOR_ENV, ALMT_SUMBER, (unsigned short *) st_sumber, hitung_ram(cek_jml_struct(SUMBER)*JML_SUMBER) );
		vPortFree (st_sumber);
	}
	
	if (almt != ENV)	{
		struct t_env *st_env;
		st_env = pvPortMalloc( sizeof (struct t_env) );
		if (st_env == NULL)	{
			printf("  GAGAL alokmem ENV !");
			return 2;
		}
		//printf("  %s(): Mallok @ %X\r\n", __FUNCTION__, st_env);
		taskENTER_CRITICAL();
		memcpy((char *) st_env, (char *) ALMT_ENV_TMP, (sizeof (struct t_env)));
		taskEXIT_CRITICAL();
		simpan_rom(SEKTOR_ENV, ALMT_ENV, (unsigned short *) st_env, hitung_ram(cek_jml_struct(ENV)) );
		vPortFree (st_env);
	}
	#ifdef PAKAI_SDCARD
	if (almt != BERKAS)	{
		struct t_file *st_file;
		st_file = pvPortMalloc( sizeof (struct t_file) );
		if (st_file == NULL)	{
			printf("  GAGAL alokmem FILE !");
			return 3;
		}
		//printf("  %s(): Mallok @ %X\r\n", __FUNCTION__, st_file);
		taskENTER_CRITICAL();
		memcpy((char *) st_file, (char *) ALMT_FILE_TMP, (sizeof (struct t_file)));
		//uprintf("jml: %d/%d\r\n", cek_jml_struct(BERKAS), hitung_ram(cek_jml_struct(BERKAS)));
		taskEXIT_CRITICAL();
		simpan_rom(SEKTOR_ENV, ALMT_FILE, (unsigned short *) st_file, hitung_ram(cek_jml_struct(BERKAS)) );
		vPortFree (st_file);
	}
	#endif
}

// flag dipake untuk data
char simpan_st_rom(int sektor, int st, int flag, unsigned short *pdata, int part)	{
	//printf("jml: %d, st: %d, ENV: %d, SMBR: %d, DATA: %d\r\n", hitung_ram(cek_jml_struct(ENV)), st, ENV, SUMBER, DATA);
	
	if (sektor==SEKTOR_ENV)		{
		kopikan_sektor_tmp(alamat_sektor(SEKTOR_ENV));
		if (st==ENV)	{
			hapuskan_sektor(sektor);
			simpan_rom(sektor, ALMT_ENV, (unsigned short *) pdata, hitung_ram(cek_jml_struct(ENV)) );
			
			kopi_semua_blok(ENV);
			#if 0
			struct t_sumber *st_sumber;
			st_sumber = pvPortMalloc( sizeof (struct t_sumber)*JML_SUMBER );
			
			if (st_sumber==NULL)	{
				printf("  GAGAL alokmem !");
				vPortFree (st_sumber);
				return 1;
			}
			
			printf("  %s(): Mallok @ %X\r\n", __FUNCTION__, st_sumber);
			taskENTER_CRITICAL();
			memcpy((char *) st_sumber, (char *) ALMT_SUMBER_TMP, sizeof (struct t_sumber)*JML_SUMBER);
			taskEXIT_CRITICAL();
			
			simpan_rom(sektor, ALMT_SUMBER, (unsigned short *) st_sumber, hitung_ram(cek_jml_struct(SUMBER)*JML_SUMBER) );
			vPortFree (st_sumber);
			#endif
		}
		
		if (st==SUMBER)	{
			hapuskan_sektor(sektor);
			simpan_rom(sektor, ALMT_SUMBER, (unsigned short *) pdata, hitung_ram(cek_jml_struct(SUMBER)*JML_SUMBER) );
			
			kopi_semua_blok(SUMBER);
			#if 0
			struct t_env *st_env;
			st_env = pvPortMalloc( sizeof (struct t_env) );
			if (st_env == NULL)	{
				printf(" %s(): ERR allok memory gagal !\r\n", __FUNCTION__);
				return 2;
			}
			printf(" %s(): Mallok @ %X\r\n", __FUNCTION__, st_env);
			taskENTER_CRITICAL();
			memcpy((char *) st_env, (char *) ALMT_ENV_TMP, (sizeof (struct t_env)));
			taskEXIT_CRITICAL();
			simpan_rom(sektor, ALMT_ENV, (unsigned short *) st_env, hitung_ram(cek_jml_struct(ENV)) );
			vPortFree (st_env);
			#endif
		}
		
		if (st==BERKAS)		{
			//uprintf("sektor: %d, almt: 0x%08X, isi: %d\r\n", sektor, ALMT_FILE, pdata[0]);
			hapuskan_sektor(sektor);
			simpan_rom(sektor, ALMT_FILE, (unsigned short *) pdata, hitung_ram(cek_jml_struct(BERKAS)) );

			kopi_semua_blok(BERKAS);
		}
	}
	else if (sektor==SEKTOR_DATA)		{
		#if 0
		if (part==1)	{					// kopi data 
			kopikan_sektor_tmp(alamat_sektor(SEKTOR_DATA));
		}
		#endif
		if (flag==1)	{
			printf("kopi sektor ke TMP\r\n");
			kopikan_sektor_tmp(alamat_sektor(SEKTOR_DATA));
			hapuskan_sektor(sektor);		// hapus sektor
		}
		if (part==0)	{					// set_data default
			simpan_rom(sektor, ALMT_DATA+(st*JML_KOPI_TEMP), (unsigned short *) pdata, hitung_ram(cek_jml_struct(DATA)*PER_SUMBER) );
		}
		else if (part==1)	{
			int i;

			printf("sektor: %d, part: %d, st: %d, flag: %d\r\n", sektor, part, st, flag);
			//for (i=0; i<ukuran_rom(SEKTOR_DATA); i++)		{
			for (i=0; i<JML_SUMBER; i++)		{
				printf(" >> i: %d, ", i);
				if (i==st)	{
					printf("data asli\r\n");
					simpan_rom(sektor, ALMT_DATA+(st*JML_KOPI_TEMP), (unsigned short *) pdata, \
						hitung_ram(cek_jml_struct(DATA)*PER_SUMBER) );
				}
				else		{
					printf("data temp\r\n");
					struct t_data *st_data;
					st_data = pvPortMalloc( PER_SUMBER*sizeof (struct t_data) );
					if (st_data == NULL)	{
						printf(" %s(): ERR allok memory gagal !\r\n", __FUNCTION__);
						vPortFree (st_data);
						return 2;
					}
					printf(" %s(): Mallok @ %X\r\n", __FUNCTION__, st_data);

					taskENTER_CRITICAL();
					memcpy((char *) st_data, (char *) ALMT_DATA_TMP+(i*JML_KOPI_TEMP), (PER_SUMBER*sizeof (struct t_data)));
					taskEXIT_CRITICAL();
					
					simpan_rom(sektor, ALMT_DATA+(i*JML_KOPI_TEMP), (unsigned short *) st_data, \
						hitung_ram(cek_jml_struct(DATA)*PER_SUMBER) );					
					vPortFree (st_data);
				}
			}
		}		
		
		#if 0
		if (flag == 0)	{		// data default
			int i=0, j;
			char *en;			

			hapuskan_sektor(sektor);
			for (i=0; i<JML_SUMBER; i++)	{
//				printf("sektor: %d, ALMT: 0x%08X, jml: %d\r\n", sektor, ALMT_DATA+(i*JML_KOPI_TEMP), hitung_ram(cek_jml_struct(DATA)*PER_SUMBER));
				simpan_rom(sektor, ALMT_DATA+(i*JML_KOPI_TEMP), (unsigned short *) pdata, hitung_ram(cek_jml_struct(DATA)*PER_SUMBER) );

				#if 0
				en = (char *) ALMT_DATA+(i*JML_KOPI_TEMP);
				for (j=0;j<40; j++)	{
					printf(" %02X", *(en++));
				}
				printf("\r\n");
				#endif
			}
			
			
			struct t_data *dt;
			dt = (char *) ALMT_DATA;
			//for (i=0; i<JML_SUMBER; i++)	
			{
				for (j=0; j<2; j++)		
				{
					printf("%d. %s, %d\r\n", j+1, dt[j].nama, dt[j].rangeL);
				}
			}
			dt = (char *) ALMT_DATA+(1*JML_KOPI_TEMP);
			//for (i=0; i<JML_SUMBER; i++)	
			{
				for (j=0; j<2; j++)		
				{
					printf("%d. %s, %d\r\n", j+1, dt[j].nama, dt[j].rangeL);
				}
			}
			
		} 
		else	{
			
		}
		#endif
	}
	
	#ifdef ADA_ASTM
	else if (sektor==SEKTOR_ASTM)		
	{
		if (flag==1)
		{
			printf("kopi sektor astm\r\n");
			kopikan_sektor_tmp(alamat_sektor(SEKTOR_ASTM));
			hapuskan_sektor(sektor);		// hapus sektor
		}
		if (part==0)	
		{					
			//simpan_rom(sektor, ALMT_VALUE_ASTM+(st*JML_KOPI_ASTM), (unsigned short *) pdata, hitung_ram(cek_jml_struct(ASTM)*PER_ASTM) );
			simpan_rom(sektor, ALMT_VALUE_ASTM+(st*JML_KOPI_ASTM), (unsigned short *) pdata, 512);
		}
		else if (part==1)	
		{
			int i;

			printf("sektor: %d, part: %d, st: %d, flag: %d\r\n", sektor, part, st, flag);
			for (i=0; i<BLOK_ASTM; i++)		
			{
				printf(" >> i: %d, ", i);
				if (i==st)	
				{
					printf("data asli\r\n");
					simpan_rom(sektor, ALMT_VALUE_ASTM+(st*JML_KOPI_ASTM), (unsigned short *) pdata, \
						hitung_ram(cek_jml_struct(ASTM)*PER_ASTM) );
				}
				else		
				{
					printf("data temp\r\n");
					//struct t_data *st_data;
					struct t_astm *st_astm;
					st_astm = pvPortMalloc( PER_ASTM*sizeof (struct t_astm) );
					if (st_astm == NULL)	
					{
						printf(" %s(): ERR allok memory gagal !\r\n", __FUNCTION__);
						vPortFree (st_astm);
						return 2;
					}
					printf(" %s(): Mallok @ %X\r\n", __FUNCTION__, st_astm);

					taskENTER_CRITICAL();
					memcpy((char *) st_astm, (char *) ALMT_DATA_TMP+(i*JML_KOPI_TEMP), (PER_ASTM*sizeof (struct t_astm)));
					taskEXIT_CRITICAL();
					
					simpan_rom(sektor, ALMT_VALUE_ASTM+(i*JML_KOPI_ASTM), (unsigned short *) st_astm, \
						hitung_ram(cek_jml_struct(ASTM)*PER_ASTM) );					
					vPortFree (st_astm);
				}
			}
		}
	}
	#endif
	
	return 0;
}

#if 0
char simpan_mem_rom(int sektor, int st, int flag, unsigned short *pdata, int def)	{
	if (sektor==SEKTOR_DATA)		{
		
		if (flag==1)	{
			kopikan_sektor_tmp(alamat_sektor(SEKTOR_DATA));
			hapuskan_sektor(sektor);		// hapus sektor
		}
		
		simpan_rom(sektor, ALMT_DATA+(st*JML_KOPI_TEMP), (unsigned short *) pdata, hitung_ram(cek_jml_struct(DATA)*PER_SUMBER) );
		
		
		#if 0
		if (flag == 0)	{		// data default
			int i=0, j;
			char *en;			

			hapuskan_sektor(sektor);
			for (i=0; i<JML_SUMBER; i++)	{
//				printf("sektor: %d, ALMT: 0x%08X, jml: %d\r\n", sektor, ALMT_DATA+(i*JML_KOPI_TEMP), hitung_ram(cek_jml_struct(DATA)*PER_SUMBER));
				simpan_rom(sektor, ALMT_DATA+(i*JML_KOPI_TEMP), (unsigned short *) pdata, hitung_ram(cek_jml_struct(DATA)*PER_SUMBER) );

				#if 0
				en = (char *) ALMT_DATA+(i*JML_KOPI_TEMP);
				for (j=0;j<40; j++)	{
					printf(" %02X", *(en++));
				}
				printf("\r\n");
				#endif
			}
			
			
			struct t_data *dt;
			dt = (char *) ALMT_DATA;
			//for (i=0; i<JML_SUMBER; i++)	
			{
				for (j=0; j<2; j++)		
				{
					printf("%d. %s, %d\r\n", j+1, dt[j].nama, dt[j].rangeL);
				}
			}
			dt = (char *) ALMT_DATA+(1*JML_KOPI_TEMP);
			//for (i=0; i<JML_SUMBER; i++)	
			{
				for (j=0; j<2; j++)		
				{
					printf("%d. %s, %d\r\n", j+1, dt[j].nama, dt[j].rangeL);
				}
			}
			
		} 
		else	{
			
		}
		#endif
		
		
	}
	
	return 0;
}
#endif


#if 0
char simpan_st_rom(int sektor, int st, int flag, unsigned short *pdata)	{
	printf("jml: %d, ENV: %d\r\n", hitung_ram(cek_jml_struct(ENV)), ENV);
	
	if (sektor==SEKTOR_ENV)		{
		if (st==ENV)	{
			hapuskan_sektor(sektor);
			simpan_rom(sektor, ALMT_ENV, (unsigned short *) pdata, hitung_ram(cek_jml_struct(ENV)) );
		}
	}
}
#endif
