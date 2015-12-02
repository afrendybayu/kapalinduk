
#ifndef __SH_SDC__
#define __SH_SDC__


#ifdef PAKAI_SDCARD

#include "tinysh/tinysh.h"


#if 0
void sh_cek_status_sdc();
void sh_cek_free_cluster();
void mount_disk(unsigned char disk);
void sh_cek_read_sdc(void);
//void sh_baca_sektor(int argc, char **argv);
void sh_ls(int argc, char **argv);
void sh_cek_pwd();

static tinysh_cmd_t cek_sdc_cmd={ 0,"cek_sdc","cek status sdc","", sh_cek_status_sdc, 0,0,0 };
static tinysh_cmd_t cek_free_cluster_cmd={ 0,"cek_free_cluster","cek free cluster sdc","", sh_cek_free_cluster, 0,0,0 };
static tinysh_cmd_t cek_read_cmd={ 0,"cek_read_sdc","cek baca sdc","", sh_cek_read_sdc, 0,0,0 };
static tinysh_cmd_t cek_pwd_cmd={ 0,"pwd","","", sh_cek_pwd, 0,0,0 };
//static tinysh_cmd_t cek_read_sektor_cmd={ 0,"baca_sektor","baca sektor sdc","", sh_baca_sektor, 0,0,0 };
static tinysh_cmd_t cek_ls_cmd={ 0,"ls","","", sh_ls, 0,0,0 };
#endif

static tinysh_cmd_t util_cd_cmd={0,"cd","change direktory","[args]", util_cd,0,0,0};
static tinysh_cmd_t util_pwd_cmd={0,"pwd","printf directory","[args]", util_pwd,0,0,0};

#endif

#endif
