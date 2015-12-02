
#ifndef __SH_ENV__
#define __SH_ENV__

#include "tinysh/tinysh.h"


void set_env_default();
void cek_env(int argc, char **argv);
char set_env(int argc, char **argv);


#ifdef PAKAI_TINYSH
static tinysh_cmd_t cek_env_cmd={ 0,"cek_env","cek konfig modul","", cek_env,0,0,0 };
static tinysh_cmd_t set_env_cmd={ 0,"set_env","set konfig modul","", set_env,0,0,0 };
#endif

#endif
