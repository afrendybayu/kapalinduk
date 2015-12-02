
#ifndef __SH_DATA__
#define __SH_DATA__

#include "tinysh/tinysh.h"

void cek_data(int argc, char **argv);
char set_data(int argc, char **argv);

#ifdef PAKAI_TINYSH
static tinysh_cmd_t cek_data_cmd={ 0,"cek_data","cek data","", cek_data,0,0,0 };
static tinysh_cmd_t set_data_cmd={ 0,"set_data","set data","", set_data,0,0,0 };
#endif

#endif
