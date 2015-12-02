

#include "FreeRTOS.h"

#ifdef PAKAI_SDCARD

#include "monita.h"
#include "ff9b/src/ff.h"
#include "sdc.h"


void cek_free_cluster()		{
	FATFS *fs;
	char ketfs[25];
    DWORD fre_clust, fre_sect, tot_sect;

    /* Get volume information and free clusters of drive 1 */
    unsigned char res;
    res = f_getfree("0:", &fre_clust, &fs);
    //ket_fs(res, &ketfs);
    //uprintf("res: %d-%s\r\n", res, ketfs);
    if (res) {
		uprintf();
		return;
		//die(res);
	}

    /* Get total sectors and free sectors */
    tot_sect = (fs->n_fatent - 2) * fs->csize;
    fre_sect = fre_clust * fs->csize;

    /* Print free space in unit of KiB (assuming 512 bytes/sector) */
	uprintf("\r\n  %lu KB total drive space.\r\n  %lu KB available.\r\n",
           tot_sect / 2, fre_sect / 2);
}

FRESULT scan_files ( char* path, int attrb )		{
    FRESULT res;
    FILINFO fno;
    DIR dir;
    int i;
    char *fn;   /* This function is assuming non-Unicode cfg. */
#if _USE_LFN
    static char lfn[_MAX_LFN + 1];
    fno.lfname = lfn;
    fno.lfsize = sizeof lfn;
#endif
	uprintf("masuk %s()\r\n", __FUNCTION__);
    res = f_opendir(&dir, path);                       /* Open the directory */
    uprintf("===> %s(), path: %s, res: %d\r\n", __FUNCTION__, path, res);
    if (res == FR_OK) {
        i = strlen(path);
        uprintf("i: %d\r\n", i);
        for (;;) {
            res = f_readdir(&dir, &fno);                   /* Read a directory item */
            uprintf("%s() --> res: %d\r\n", __FUNCTION__, res);
            if (res != FR_OK || fno.fname[0] == 0) break;  /* Break on error or end of dir */
            if (fno.fname[0] == '.') continue;             /* Ignore dot entry */
#if _USE_LFN
            fn = *fno.lfname ? fno.lfname : fno.fname;
#else
            fn = fno.fname;
#endif
            if (fno.fattrib & AM_DIR) {                    /* It is a directory */
                sprintf(&path[i], "/%s", fn);
                res = scan_files(path, attrb);
                if (res != FR_OK) break;
                path[i] = 0;
            } else {                                       /* It is a file. */
                printf("%s/%s\n", path, fn);
            }
        }
    }

    return res;
}



#endif
