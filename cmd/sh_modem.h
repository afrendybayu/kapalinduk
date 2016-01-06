
#ifndef __SH_MODEM__
#define __SH_MODEM__

#include "tinysh/tinysh.h"

void cek_modem();
void set_modem_echo();
//char set_modem(int argc, char **argv);

#ifdef PAKAI_TINYSH


static tinysh_cmd_t cek_modem_cmd={ 0,"cek_modem","cek modem","", cek_modem,0,0,0 };
static tinysh_cmd_t set_modem_echo_cmd={ 0,"set_modem_echo","set modem echo","", set_modem_echo,0,0,0 };
//static tinysh_cmd_t set_modem_cmd={ 0,"set_modem","set modem","", 0,0,0,0 };
#endif

#endif
