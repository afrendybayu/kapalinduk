
// Afrendy Bayu
// Depok, 29 Juli 2013
// 



#include "FreeRTOS.h"
#include "task.h"
#include "monita.h"

#include "sh_utils.h"
#include "sh_sumber.h"
#include "manual.h"
#include "iap.h"

void cek_sumber(int argc, char **argv)		{
	IAP_return_t iap_return;
	iap_return = iapReadBlankSector(SEKTOR_ENV, SEKTOR_ENV);
	if (iap_return.ReturnCode == CMD_SUCCESS)	{
		printf("  ERR: Sektor Kosong ! Lakukan: set_sumber default\r\n");
		return;
	}
	
	int i=0;
	struct t_sumber *st_sumber;
	st_sumber = (char *) ALMT_SUMBER;

	uprintf("\r\n  Cek Sumber\r\n  ******************************\r\n");
	uprintf (" no |        Nama       |        IP       | Almt |  Status  | Formula\r\n");
	for (i=0; i<JML_SUMBER; i++)	{
		printf(" %2d | %-17s | %3d.%3d.%3d.%3d | %4d |  %-6s  | %-19s\r\n", i+1, st_sumber[i].nama, \
			st_sumber[i].IP0, st_sumber[i].IP1, st_sumber[i].IP2, st_sumber[i].IP3,	\
			st_sumber[i].alamat, (st_sumber[i].status==0)?"Mati":"Aktif", st_sumber[i].form);
	}
}

#if 0
void cek_sumber_temp(int argc, char **argv)		{
	int i=0;
	struct t_sumber *st_sumber;
	st_sumber = (char *) ALMT_SUMBER_TMP;
	
	uprintf("\r\n  Cek Sumber: 0x%08X\r\n  ******************************\r\n", ALMT_SUMBER_TMP);
	uprintf (" no |        Nama       |        IP       | Almt | Stack |      Keterangan     |\r\n");
	for (i=0; i<JML_SUMBER; i++)	{
		printf(" %2d | %-17s | %3d.%3d.%3d.%3d | %4d |  %3d  | %-19s |\r\n", i+1, st_sumber[i].nama, \
			st_sumber[i].IP0, st_sumber[i].IP1, st_sumber[i].IP2, st_sumber[i].IP3,	\
			st_sumber[i].alamat, st_sumber[i].stack, st_sumber[i].ket);
	}
}
#endif

char set_sumber(int argc, char **argv)		{
	if (argc>4 || argc==1)		sumber_kitab();
	if (argc==2 || argc==3)	{
		printf("\r\n");
		if (strcmp(argv[1], "default") == 0)	{
			printf("  set_sumber dengan konfig default !\r\n");
			set_sumber_default();
			return 0;
		}
		sumber_kitab();
		return 1;
	}

	int no = cek_nomor_valid(argv[1], JML_SUMBER);
	no--;
	if (no == TIDAK_VALID)	{
		printf("  no sumber TIDAK VALID\r\n");
		return 2;
	}
	
	struct t_sumber *st_sumber;
	st_sumber = pvPortMalloc( JML_SUMBER * sizeof (struct t_sumber) );
	if (st_sumber == NULL)	{
		printf(" %s(): ERR allok memory gagal !\r\n", __FUNCTION__);
		vPortFree(st_sumber);
		return -1;
	}
	printf(" -->%s(): Mallok @%X\r\n", __FUNCTION__, st_sumber);
	taskENTER_CRITICAL();
	memcpy((char *) st_sumber, (char *) ALMT_SUMBER, (JML_SUMBER * sizeof (struct t_sumber)));
	taskEXIT_CRITICAL();
	
	if (argc==4)	{
		printf("\r\n");
		if (strcmp(argv[2], "nama") == 0)	{
			printf("  set Nama sumber ");
			if (strlen(argv[2]) > sizeof (st_sumber[no].nama))		{
				printf("terlalu panjang !");
				return 3;	
			}
			else	{
				sprintf(st_sumber[no].nama, "%s", argv[3]);
				printf("[%d]: %s\r\n", no, st_sumber[no].nama);
			}
		}
		#ifdef PAKAI_ETH
		else if (strcmp(argv[2], "ipaddr") == 0)  	{
			printf("  set IP ");  
			
			uint ret_ip = baca_ip( argv[3] );
			if (ret_ip > 0)	{
				st_sumber[no].IP0 = (unsigned char)(ret_ip >> 24);
				st_sumber[no].IP1 = (unsigned char)(ret_ip >> 16);
				st_sumber[no].IP2 = (unsigned char)(ret_ip >> 8);
				st_sumber[no].IP3 = (unsigned char)(ret_ip);
				printf(":%d.%d.%d.%d\r\n", \
					st_sumber[no].IP0, st_sumber[no].IP1, st_sumber[no].IP2, st_sumber[no].IP3);
			} 
			else		{
				printf("ERROR !!\r\n");
				return 2;
			}
		}
		#endif
		else if (strcmp(argv[2], "ket") == 0)	{
			printf("  Set Keterangan ");

			if (strlen(argv[3]) > sizeof (st_sumber[no].ket))		{
				printf("terlalu panjang !\r\n");
				return 1;	
			}
			else	{
				sprintf(st_sumber[no].ket, "%s", argv[3]);
				printf(": %s\r\n", st_sumber[no].ket);
			}
		}
		else if (strcmp(argv[2], "tipe") == 0)	{
			st_sumber[no].tipe = atoi( argv[3] );
			printf("  Tipe:%d\r\n", st_sumber[no].tipe);
			
		}
		else if (strcmp(argv[2], "stack") == 0)	{
			st_sumber[no].stack = atoi( argv[3] );
			printf("  Tipe:%d\r\n", st_sumber[no].stack);
			
		}
		else if (strcmp(argv[2], "alamat") == 0)	{
			st_sumber[no].alamat = atoi( argv[3] );
			printf("  Alamat:%d\r\n", st_sumber[no].alamat);
			
		}
		else if (strcmp(argv[2], "formula") == 0)	{
			sprintf(st_sumber[no].form, "%s", argv[3]);
 			st_sumber[no].validForm = 0;
			printf("  Formula:%d\r\n", st_sumber[no].form);
			
		}
		else if (strcmp(argv[2], "status") == 0)	{
			st_sumber[no].status = atoi( argv[3] );
			printf("  Status:%d\r\n", st_sumber[no].status);
			
		}
		
	}
	//printf("\r\n");
	
	//simpan_struct_block_rom(SEKTOR_ENV, SUMBER, 0, (char *) st_sumber);
	simpan_st_rom(SEKTOR_ENV, SUMBER, 1, (unsigned short *) st_sumber, 0);
	vPortFree(st_sumber);
	
	return 0;
}

void set_sumber_default()		{
	int i;
	
	//struct t_sumber st_sumber[JML_SUMBER];
	struct t_sumber *st_sumber;
	
	st_sumber = pvPortMalloc( sizeof (struct t_sumber)*JML_SUMBER );
	
	if (st_sumber==NULL)	{
		printf("  GAGAL alokmem !");
		vPortFree (st_sumber);
		return;
	}
	
	printf("  %s(): Mallok @ %X\r\n", __FUNCTION__, st_sumber);
	for (i=0; i<JML_SUMBER; i++)	{
		sprintf(st_sumber[i].nama, "Sumber %d", i+1);
		st_sumber[i].IP0 = 192;
		st_sumber[i].IP1 = 168;
		st_sumber[i].IP2 = 1;
		st_sumber[i].IP3 = 250;
		st_sumber[i].stack  = 0;
		st_sumber[i].alamat = 0;
		st_sumber[i].tipe = 0;
		st_sumber[i].status = 0;
		strcpy(st_sumber[i].form, "");
		strcpy(st_sumber[i].ket, "---");
	}
	
	//simpan_struct_block_rom(SEKTOR_ENV, SUMBER, 1, (char *) st_sumber);
	simpan_st_rom(SEKTOR_ENV, SUMBER, 0, (unsigned short *) st_sumber, 0);
	vPortFree (st_sumber);
}

