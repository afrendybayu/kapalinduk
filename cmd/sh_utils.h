
#ifndef __SH_UTILS__
#define __SH_UTILS__

#include "tinysh/tinysh.h"

int cek_nomor_valid(char *str, int max);
unsigned int baca_ip(char *sIP);
void cek_struct(int argc, char **argv);
int cek_jml_struct(char no);
int cek_no_struct(char *str);
int nIstilah(char **fruits);
int parsing_istilah(char *strx, char **istilah);


#ifdef PAKAI_TINYSH
	static tinysh_cmd_t cek_struct_cmd = { 0,"cek_struct","cek struct","", cek_struct, 0,0,0 };
#endif


#endif
