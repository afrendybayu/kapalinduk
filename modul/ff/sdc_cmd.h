
#ifndef __SH_SDC_CMD__
#define __SH_SDC_CMD__


#ifdef PAKAI_SDCARD

#include "ff9b/src/ff.h"
//#include "tinysh/tinysh.h"

void cek_free_cluster();
FRESULT scan_files ( char* path, int attrb );

#endif

#endif
