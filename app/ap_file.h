
#ifndef __AP_FILE__
#define __AP_FILE__



#include "FreeRTOS.h"
#include "tinysh/tinysh.h"

#ifdef PAKAI_FILE_SIMPAN
void simpan_file_data()	;	
void tulis_konfig_file(char *s, FIL* fp);
int hapus_folder(char *fol);
int simpan_konfig(int argc, char **argv);
int upload_konfig(char *path);

static tinysh_cmd_t simpan_file_cmd={0,"simpan_konfig","simpan konfig ke file","[args]", simpan_konfig,0,0,0};
#endif
#endif
