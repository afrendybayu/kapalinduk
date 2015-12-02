
#include "FreeRTOS.h"
#include "task.h"
//#include "serial/tinysh.h"
#include "sh_hardware.h"
#include "cpu_setup.h"
#include "monita.h"
#include "iap.h"
#include "manual.h"
#include "sh_utils.h"

//extern struct t_st_hw st_hw;



void idle_tick()	{
	qsprintf("\r\n  idle tick : %d/s, adc: %d/s\r\n", st_hw.idle, st_hw.adc_pd);
}



void cek_blank_sector_rom()		{
	IAP_return_t iap_return;
	uchr i;
	uprintf("\r\n  Cek Kondisi Sektor ROM\r\n  ***********************************************\r\n");
	for (i=0; i<JML_SECTOR_LPC; i++)	{
		iap_return = iapReadBlankSector(i, i);
		if (iap_return.ReturnCode == 0)	{
			uprintf("  Sektor %2d kosong[%2dKB] : %08X - %04X\r\n", i, ukuran_rom(i), iap_return.Result[0], iap_return.Result[1]);
		} else if (iap_return.ReturnCode == SRC_ADDR_ERROR) {
			uprintf("  Sektor %2d ISI[2] : %08X - %04X\r\n", i, iap_return.Result[0], iap_return.Result[1]);
		} else if (iap_return.ReturnCode == SECTOR_NOT_BLANK) {
			uprintf("  Sektor %2d ISI[NOT_BLANK] : mulai sektor %02d - %04X\r\n", i, iap_return.Result[0], iap_return.Result[1]);
		} else {
			uprintf("  Error no : %d\r\n", iap_return.ReturnCode);
		}
	}
}

void hapus_sector_rom(int argc, char **argv)			{
	printf("\r\n");
	if (argc != 2)	{
		//iap_kitab();
		return;
	}

	// menghindari argumen ngawur/bukan angka
	if (atoi(argv[1])==0)	return;
	
	int nSktr = cek_nomor_valid(argv[1], JML_SECTOR_LPC-1);
	char hsl = hapuskan_sektor(nSktr);
	printf("  Hapus Sektor %d ", nSktr);
	if (hsl==0)		printf("BERHASIL\r\n");
	else 			printf("GAGAL !!\r\n");
	
	#if 0
	IAP_return_t iap_return = iapSiapSektor(nSktr, nSktr);
	
	if ( iap_return.ReturnCode == CMD_SUCCESS )	{
		printf("  SS%d", nSktr);
		iap_return = iapEraseSector(nSktr, nSktr);
	}

	if (iap_return.ReturnCode == CMD_SUCCESS)	
		printf("  HS%d.\r\n", nSktr);
	else
		printf("  GAGAL Hapus Sektor %d !!\r\n", nSktr);
	#endif
}



void simpan_struct_rom(int argc, char **argv)			{		// bisa dihapus ????
	printf("\r\n");
	struct t_sumber st_sumber[JML_SUMBER];
	struct t_env *st_env;
	
	//struct t_env *pst_env;
	//struct t_sumber *pst_sumber;
	//struct t_cron *pst_cron;
	
	char *pdata1, *pdata2, *pdata3;
	char *pch1, *pch2, *pch3;
	int  alm1, alm2, alm3;
	int	 jml1, jml2, jml3;
	int ndata=-1, st=-1, jml, hapus;
	
	char* istilah[] = {"data", "env", "sumber", "cron", NULL};
	int i = parsing_istilah(argv[1], istilah);
	if (i>=0)	{
		if ( argc>2 && strcmp("data", argv[1])==0 )	{
			ndata = atoi(argv[2]);
			st = 0;		// data
			printf("Simpan data: %d!!\r\n", ndata);
		} else {
			st = 1;		// env, sumber
			printf("Simpan env sumber: %d!!\r\n", st);
		}
	} else return;
	
	if (st == 1)	{		// env
		// simpan_data_rom(nfmt, sektor, addr1, 4, "merdeka", addr2, 232, "jayaindonesia");
		hapus = 1;
		jml = 2 + 3*2;
		alm1 = ALMT_ENV;
		alm2 = ALMT_SUMBER;
		jml1 = cek_jml_struct(ENV);
		jml2 = cek_jml_struct(SUMBER)*JML_SUMBER;
		pch1 = &st_env;
		pch2 = &st_sumber[0];
		
		// ENV
		pdata1 = pvPortMalloc(jml1);
		if (pdata1!=NULL)	{
			taskENTER_CRITICAL();
			memcpy(pdata1, pch1, jml1);
			taskEXIT_CRITICAL();
			//printf("  memSIP : %d, Alm: 0x%08X.\r\n", jml1, alm1);
		} else {
			printf("  GAGAL alokmem !\r\n");
			vPortFree (pch1);
			return;
		}
		
		// SUMBER
		pdata2 = pvPortMalloc(jml2);
		if (pdata2!=NULL)	{
			taskENTER_CRITICAL();
			memcpy(pdata2, pch2, jml2);
			taskEXIT_CRITICAL();
			//printf("  memSIP : %d, Alm: 0x%08X.\r\n", jml2, alm2);
		} else {
			printf("  GAGAL alokmem !\r\n");
			vPortFree (pch1);
			vPortFree (pch2);
			//vPortFree (pch2);
			return;
		}
		
		simpan_data_rom(jml, SEKTOR_ENV, hapus, alm1, jml1, (unsigned short *)pch1, alm2, jml2, (unsigned short *)pch2);
		vPortFree (pch1);
		vPortFree (pch2);
	}
	else if (st == 0)	{		// data
		jml = 2 + 3*1;
		alm1 = ALMT_DATA;
		jml1 = cek_jml_struct(DATA)*PER_SUMBER;
		int j;
		
		// DATA
		for (j=0; j<JML_SUMBER; j++)	{
//			pch1 = &st_data[i];
			pdata1 = pvPortMalloc(jml1);
			if (pdata1!=NULL)	{
				taskENTER_CRITICAL();
				memcpy(pdata1, pch1, jml1);
				taskEXIT_CRITICAL();
				printf("  memSIP : %d, Alm: 0x%08X.\r\n", jml1, alm1);
			} else {
				printf("  GAGAL alokmem !\r\n");
				vPortFree (pch1);
				return;
			}
			if (i==0)	hapus = 1;
			else 		hapus = 0;
			
			simpan_data_rom(jml, SEKTOR_DATA, hapus, alm1, jml1, (unsigned short *)pch1);
			vPortFree (pch1);
		}
	}
}



void simpan_sector_rom(int argc, char **argv)			{
	printf("\r\n");

	struct t_sumber st_sumber[JML_SUMBER];
	struct t_env *st_env;
	struct t_data st_data[JML_SUMBER];

	char *pdata, *pch;
	int jml = 0, nn=256, sektor = SEKTOR_ENV;
	int ndata=-1, st=-1;
	
	char* istilah[] = {"data", "env", "sumber", "cron", NULL};
	int i = parsing_istilah(argv[1], istilah);
	if (i>=0)	{
		if ( argc>2 && strcmp("data", argv[1])==0 )	{
			ndata = atoi(argv[2]);
			//printf("Simpan data sumber: %d!!\r\n", ndata);
		} else {
			st = i;
			//printf("Simpan: %d!!\r\n", st);
		}
	} else return;


	unsigned int alamat = 1024*(st-1) + ALMT_SEKTOR_21;
	if (st==SUMBER)		{
		jml = cek_jml_struct(SUMBER)*JML_SUMBER;
		pch = &st_sumber[0];
		printf("  Simpan SumberROM\r\n");
	}
	else if (st==ENV)	{
		jml = cek_jml_struct(ENV);
		pch = &st_env;
		printf("  Simpan EnvROM\r\n");
	}

	if (ndata>=0)	{
		alamat = 1024*ndata + ALMT_SEKTOR_20;
		sektor = SEKTOR_DATA;
		jml = cek_jml_struct(DATA)*PER_SUMBER;
		pch = &st_data[0];
		printf("  Simpan DataROM\r\n");
	}

	if (jml > 256)  nn =  512;
	if (jml > 512)  nn = 1024;
	if (jml > 1024) nn = 4096;
	
	pdata = pvPortMalloc(jml);
	if (pdata!=NULL)	{
		taskENTER_CRITICAL();
		memcpy(pdata, pch, jml);
		taskEXIT_CRITICAL();
		printf("  memSIP : %d/%d, Alm: 0x%08X.\r\n", jml, nn, alamat);
	} else {
		printf("  GAGAL alokasi memori !!\r\n");
		vPortFree (pdata);
		return;
	}

	hapuskan_sektor(sektor);
	simpan_rom(sektor, alamat, (unsigned short *)pdata, nn);	
	//simpan_rom(sektor, alamat, (unsigned short *)pch, nn);	
	
	printf("  data: ");
	for (i=0; i<10; i++)	{
		printf("%02X ", pdata[i]);
	}
	printf("\r\n");
	
	vPortFree (pdata);
}

char kopi_sektor(int argc, char **argv)	{
	if (argc!=2)	{
		printf("\r\n kopi_sektor [SEKTOR SUMBER]\r\n");
		return;
	}
	
	int hasil, almt = atoi(argv[1]);
	if (almt<10)	return;
	
	printf("\r\n  Kopi_sektor %d ke sektor %d: 0x%08X\r\n", almt, SEKTOR_TEMP, alamat_sektor(almt));
	return kopikan_sektor_tmp(alamat_sektor(almt));
}

void baca_rom()	{
	struct t_env *penv;
	penv = ALMT_ENV;
	
	printf("\r\n  almt: %08X, board: %s, SN: %s, prio: %d\r\n", ALMT_ENV, penv->nama_board, penv->SN, penv->prioDebug);
	
	struct t_env *penx;
	char *aaa;
	aaa = (char *) ALMT_ENV;
	
	
	int i;
	char *envx;
	envx = (char *) ALMT_FILE;
	printf("  data: ");
	for (i=0; i<20; i++)	{
		printf("%02X ", *envx++);
	}
	printf("\r\n");
}
