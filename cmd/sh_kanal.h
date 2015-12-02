
#ifndef __SH_KANAL__
#define __SH_KANAL__

#include "tinysh/tinysh.h"

void cek_kanal();
char set_kanal(int argc, char **argv);

#ifdef PAKAI_TINYSH
static tinysh_cmd_t cek_kanal_cmd={ 0,"cek_kanal","cek kanal","", cek_kanal,0,0,0 };
static tinysh_cmd_t set_kanal_cmd={ 0,"set_kanal","set kanal","", set_kanal,0,0,0 };
#endif

#endif
