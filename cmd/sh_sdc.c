
#include "FreeRTOS.h"
#include "task.h"


#ifdef PAKAI_SDCARD

#include "monita.h"
#include "ff/ff9b/src/ff.h"
#include "ff/sdc.h"
#include "sh_sdc.h"
#include "ff/sdc_cmd.h"


//extern FATFS xFatFs[];


void sh_cek_status_sdc()	{
	qsprintf("status : %s\r\n", (cek_status_sdc()==0)?"AKTIF":"MATI" );
}

#if 0
void mount_disk(unsigned char disk)	{
	//FATFS xFatFs[2];
	char strFs[24];
	unsigned char a;
	a = f_mount(disk, &xFatFs[0]);
	//a = f_mount(disk, &xFatFs[0]);
	//ket_fs(a, &strFs);
	//qsprintf("a: %d-%s, xFatFs.drive: %d, size: %d\r\n", a, ket_fs(a), xFatFs[0].drv, xFatFs[0].csize);
	//qsprintf("a: %d-%s\r\n", a, strFs);
}
#endif
void sh_cek_read_sdc(void)	{
	unsigned char data[2];
	qsprintf("====> baca %s()\r\n", __FUNCTION__);
	sdc_read(data, 0, 1);
}

void sh_cek_free_cluster()	{
	cek_free_cluster();
}

void sh_ls(int argc, char **argv)		{
	scan_files ("0:/", 0);
}

void sh_cek_pwd()	{
	char buff[60];
	
	f_getcwd (buff, 60);     /* [IN] The length of the buffer */
	printf("\r\npwd: %s\r\n", buff);
}

void sh_baca_sektor(int argc, char **argv)		{
	if (argc!=3)	{
		qsprintf("argumen KURANG, baca_sektor [no] [Nsektor]\r\n");
	}
	
	int skr=atoi(argv[1]);
	int pjg=atoi(argv[2]);
	char isi[512];
	qsprintf("---> baca_sektor %d, sebanyak %d\r\n", skr, pjg);
	vTaskDelay(10);
	sdc_read(isi, skr, pjg);
}
#endif
