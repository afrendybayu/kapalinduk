
// Afrendy Bayu
// Depok, 28 Sept 2013
// 

#include "FreeRTOS.h"
#include "task.h"
#include "monita.h"

#include "sh_file.h"
//#include "sh_utils.h"
#include "manual.h"
#include "iap.h"

//extern struct t_env env;
#ifdef PAKAI_FILE_SIMPAN

void set_konfig_file(int argc, char **argv)	{
	if (argc!=2)		{
	//	file_kitab();
		return;
	}
	
	uprintf("\r\n Upload Konfig, path: %s\r\n", argv[1]);
	upload_konfig(argv[1]);
	uprintf("\r\n ******* SUKSES ! ke file %s\r\n", argv[1]);
}

void cek_file(int argc, char **argv)	{
	IAP_return_t iap_return;
	iap_return = iapReadBlankSector(SEKTOR_ENV, SEKTOR_ENV);
	if (iap_return.ReturnCode == CMD_SUCCESS)	{
		printf("  ERR: Sektor Kosong ! Lakukan: set_file default\r\n");
		return;
	}
	
	int i=0, j, k;
	//struct t_sumber *st_sumber;
	struct t_file  *st_file;
	st_file = (char *) ALMT_FILE;
	
	struct t_data *st_data;
	
	//uprintf("\r\n  Cek modul %s %s   \r\n  ******************************\r\n", BOARD_SANTER, BOARD_SANTER_versi);
	uprintf("\r\n  Simpan : %s\r\n", (st_file->simpan==YA)?"YA [1]":"TIDAK [0]");
	uprintf("  File   : %d data\r\n", st_file->jml);
	if (st_file->jml>0)
		uprintf(" Urut  | NO |  ID  |          NAMA           :     Nilai     |\r\n");
	for (i=0; i<JML_TITIK_DATA; i++)	{
		if (st_file->urut[i]==0)			break;
		else {
			j = (int) ((st_file->urut[i]-1)/PER_SUMBER);
			k = (st_file->urut[i]-1) % PER_SUMBER;
			st_data = ALMT_DATA + j*JML_KOPI_TEMP;
			uprintf("  %3d.\t%3d  [%4d] --> %-20s : %.1f\r\n", i+1, \
				st_file->urut[i], st_data[k].id, st_data[k].nama, data_f[st_file->urut[i]-1]);
		}
	}
}

char set_file(int argc, char **argv)	{
	if (argc>3 || argc==1)		{
	//	file_kitab();
	}
		
	if (argc==2)	{
		uprintf("\r\n");
		if (strcmp(argv[1], "help") == 0)	{
			//file_kitab();
			return 0;
		}
		else if (strcmp(argv[1], "default") == 0)	{
			uprintf("  set file dengan konfig default !\r\n");
			set_file_default();
			return 0;
		}
		return 1;
	}
	
	
	printf("\r\n");
	struct t_file *st_file;
	st_file = pvPortMalloc( sizeof (struct t_file) );
	if (st_file == NULL)	{
		printf(" %s(): ERR allok memory gagal !\r\n", __FUNCTION__);
		vPortFree (st_file);
		return 2;
	}
	//printf("  %s(): Mallok @ %X\r\n", __FUNCTION__, st_file);
	memcpy((char *) st_file, (char *) ALMT_FILE, (sizeof (struct t_file)));
	
	int urut, i, j, k;
	if (argc==3)	{
		int no;
		
		if (!strcmp(argv[1], "tambah") || !strcmp(argv[1], "+"))	{
			no = cek_nomor_valid(argv[2], JML_TITIK_DATA);
			for (i=0; i<JML_TITIK_DATA; i++)	{
				if (st_file->urut[i]==no)		{
					uprintf("  DATA no-%d SUDAH ADA !\r\n", no+1);
					return;
				}
				
				if (st_file->urut[i]==0)			break;
				else {
					uprintf("  No %3d --> %d\r\n", i+1, st_file->urut[i]);
				}
			}
			urut = i;
			//uprintf("urut: %d, i: %d, no: %d\r\n", urut, i, no);
			st_file->urut[i] = no;
			uprintf("  No %3d --> tambah Data %d\r\n", i+1, st_file->urut[i]);
			st_file->jml++;
		}
		else if (!strcmp(argv[1], "hapus") || !strcmp(argv[1], "-"))	{
			no = cek_nomor_valid(argv[2], JML_TITIK_DATA);
			int  fx, sisa=0;
			
			for (i=0; i<JML_TITIK_DATA; i++)	{
				if (st_file->urut[i]==no)		{
					fx = i;
					st_file->jml--;
					sisa = st_file->jml-i;
					st_file->urut[i] = 0;
				}
			}
			// 5 3
			for (i=0; i<sisa; i++)		{
				st_file->urut[fx+i] = st_file->urut[fx+i+1];
			}
			st_file->urut[fx+i] = 0;
			uprintf("  data no : %d terhapus, %d\r\n", no, fx+i);
		}
		else if (strcmp(argv[1], "simpan") == 0)	{
			int p = strlen(argv[2]);
			if (p==1)
				st_file->simpan = atoi(argv[2]);
			else 	{
				if ( !strcmp(argv[2],"aktif") || !strcmp(argv[2],"ya") )	
					st_file->simpan = YA;
				else
					st_file->simpan = TIDAK;
			}
		}
	}
	
	//simpan_struct_block_rom(SEKTOR_ENV, ENV, 0, (char *) st_env);
	simpan_st_rom(SEKTOR_ENV, BERKAS, 1, (unsigned short *) st_file, 0);
	vPortFree (st_file);
	return 0;
	
}

void set_file_default()		{
	int i;
	
	struct t_file *st_file;
	st_file = pvPortMalloc( sizeof (struct t_file) );

	if (st_file==NULL)	{
		printf("  GAGAL alokmem !");
		vPortFree (st_file);
		return;
	}
	
	printf("  %s(): Mallok @ %X\r\n", __FUNCTION__, st_file);
	st_file->jml = 0;
	for (i=0; i<JML_TITIK_DATA; i++)	{
		st_file->urut[i] = 0;
	}
	
	simpan_st_rom(SEKTOR_ENV, BERKAS, 0, (unsigned short *) st_file, 0);

}

#endif

