
#ifndef __SH_SUMBER__
#define __SH_SUMBER__

#include "tinysh/tinysh.h"


void set_sumber_default();
void cek_sumber(int argc, char **argv);
char set_sumber(int argc, char **argv);
void cek_sumber_temp(int argc, char **argv);

#ifdef PAKAI_TINYSH
static tinysh_cmd_t cek_sumber_cmd={ 0,"cek_sumber","cek sumber modul","", cek_sumber,0,0,0 };
static tinysh_cmd_t set_sumber_cmd={ 0,"set_sumber","set sumber modul","", set_sumber,0,0,0 };

#if 0
static tinysh_cmd_t cek_sumber_temp_cmd={ 0,"cek_sumber_temp","cek sumber modul","", cek_sumber_temp,0,0,0 };
#endif
#endif

#endif
