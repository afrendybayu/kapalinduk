
#include "FreeRTOS.h"
#include "monita.h"
#include "sh_utils.h"
#include <stdio.h>

int cek_nomor_valid(char *str, int max)	{
	int nx = atoi(str);
	if (nx>=0 && nx<=max)	return nx;
	else return TIDAK_VALID;
}

unsigned int baca_ip(char *sIP)	{
	unsigned char byte = atoi(sIP);
	unsigned ip, i=2;
	char *pch;
	
	if (byte==0)	{
		printf("  ERROR parsing IP !!\r\n");
		return 0;
	}
	ip = byte<<24;	
	pch = strchr(sIP,'.');
	while (pch!=NULL)	{
		byte = atoi(pch+1);
		//printf("pch: %s, %d, %d\r\n", pch+1, atoi(pch+1), byte);
		ip |= byte<<(i--*8);
		pch = strchr(pch+1,'.');
	}
	return ip;
}

int cek_no_struct(char *str)	{
	if (strcmp(str, "env")==0)
		return ENV;
	//else if (strcmp(str, "data")==0)
	//	return DATA;
	else if (strcmp(str, "sumber")==0)
		return SUMBER;
	else 
		return -1;
}


int cek_jml_struct(char no)	{
	if (no == SUMBER)
		return (sizeof(struct t_sumber));
	else if (no == ENV)
		return (sizeof(struct t_env));
	else if (no == DATA)
		return (sizeof(struct t_data));
	//else if (no == ASTM)
		//return (sizeof(struct t_astm));
		
	#ifdef PAKAI_SDCARD
	else if (no == BERKAS)
		return (sizeof(struct t_file));
	#endif
	else 
		return 0;
}

void cek_struct(int argc, char **argv)		{
	if (argc>2)	{
		return;
	}
	
	int st = cek_jml_struct(cek_no_struct(argv[1]));
	if (argc==1)	{
		printf("\r\n");
		printf("  Jml struct env   : %d\r\n", cek_jml_struct(ENV));
		printf("  Jml struct data  : %d\r\n", (cek_jml_struct(DATA)*PER_SUMBER));
		printf("  Jml struct sumber: %d\r\n", (cek_jml_struct(SUMBER)*JML_SUMBER));
		printf("  Jml struct file  : %d\r\n", (cek_jml_struct(BERKAS)));
		return;
	}
	printf("  Jml struct %s : %d\r\n", argv[1], st);
}

int nIstilah(char **fruits)	{
	int i = 0;
	while (fruits[i] != NULL) {
		//printf ("%s ",*(fruits+i));
		i++;
	}
	return i;
}

int parsing_istilah(char *strx, char **istilah)	{
	int i, k=-1;
	char *pch;
	int jmlIs = nIstilah(istilah);
	
	for(i=0; i<jmlIs; i++)	{
		//printf(">>%d -- %s\r\n", i+1, istilah[i]);
		pch = strstr (strx, istilah[i]);
		if (pch != NULL)	{
			//printf("%d, pch: %d\r\n", i+1, *pch);
			//printf("posisi: %s\r\n", pch);
			k=i;
			break;
			
			//aksi(i, pch);
		}
	}
	return k;
}

void status_kanal(char *ket, int st)		{
	
}
