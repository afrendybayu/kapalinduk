
#ifndef __SH_SDC__
#define __SH_SDC__


//
#include "FreeRTOS.h"
#include "tinysh/tinysh.h"

#ifdef PAKAI_SDCARD

int cek_fs_free(void);
void set_fs_mount(void);
int util_cd(int argc, char **argv);
int util_pwd();
int util_ls(int argc, char **argv);
int util_view (int argc, char **argv);
void util_mkdir(int argc, char **argv);
int util_rm (int argc, char **argv);

static tinysh_cmd_t util_cd_cmd={0,"cd","change direktory","[args]", util_cd,0,0,0};
static tinysh_cmd_t util_pwd_cmd={0,"pwd","printf directory","[args]", util_pwd,0,0,0};
static tinysh_cmd_t util_view_cmd={0,"view","simple text viewer","[args]", util_view,0,0,0};
static tinysh_cmd_t util_ls_cmd={0,"ls","list direktory","[args]", util_ls,0,0,0};
static tinysh_cmd_t util_mkdir_cmd={0,"mkdir","buat direktory","[args]", util_mkdir,0,0,0};
static tinysh_cmd_t util_rm_cmd={0,"rm","hapus file/folder","[args]", util_rm,0,0,0};

#endif

#endif
