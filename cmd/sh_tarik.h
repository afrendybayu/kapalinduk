
#ifndef __SH_TARIK__
#define __SH_TARIK__

#include "tinysh/tinysh.h"

void tarik_data();

#ifdef PAKAI_TINYSH
static tinysh_cmd_t tarik_data_cmd={ 0,"tarik","tarik","", tarik_data,0,0,0 };
#endif

#endif
