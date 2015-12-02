
#ifndef __SH_RELAY__
#define __SH_RELAY__

#include "tinysh/tinysh.h"

void cek_relay(int argc, char **argv);
char set_relay(int argc, char **argv);

#ifdef PAKAI_TINYSH
static tinysh_cmd_t cek_relay_cmd={ 0,"cek_relay","cek data","", cek_relay,0,0,0 };
static tinysh_cmd_t set_relay_cmd={ 0,"set_relay","set data","", set_relay,0,0,0 };
#endif

#endif

