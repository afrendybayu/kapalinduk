
//#include <stdio.h>
#include "FreeRTOS.h"
//#include "sh_serial.h"
#include "cpu_setup.h"
#include "tinysh/tinysh.h"

#ifndef __SH_HARDWARE__
#define __SH_HARDWARE__

void idle_tick();
void cek_blank_sector_rom();
void hapus_sector_rom();
void simpan_sector_rom(int argc, char **argv);

void baca_rom();
void simpan_struct_rom(int argc, char **argv);
char kopi_sektor(int argc, char **argv);

#ifdef PAKAI_TINYSH
static tinysh_cmd_t reset_cmd={0,"reset","reset cpu","[args]", reset_cpu,0,0,0};
static tinysh_cmd_t idle_tick_cmd={ 0,"idle","lihat idle sistem","", idle_tick,0,0,0 };
static tinysh_cmd_t sektor_free_cmd={ 0,"cek_sektor","lihat memori ROM ","", cek_blank_sector_rom,0,0,0 };
static tinysh_cmd_t hapus_sektor_rom_cmd={ 0,"hapus_sektor_rom","hapus memori ROM ","", hapus_sector_rom,0,0,0 };
static tinysh_cmd_t simpan_sektor_rom_cmd={ 0,"simpan_sektor_rom","simpan memori ROM ","", simpan_sector_rom,0,0,0 };

static tinysh_cmd_t baca_rom_cmd={ 0,"baca_rom","baca memori ROM ","", baca_rom,0,0,0 };
static tinysh_cmd_t simpan_struct_rom_cmd={ 0,"simpan_struct","baca memori ROM ","", simpan_struct_rom,0,0,0 };

static tinysh_cmd_t kopi_sektor_cmd={ 0,"kopi_sektor","kopi_sektor ROM ","", kopi_sektor,0,0,0 };

#endif

#endif

#if 0
//	iTong$ task_list 
//	Task      State  Priority  Stack	#
//	************************************************
//	Shell		R		2		2537	1
//	IDLE		R		0		82		5
//	Serial2		B		2		470		2
//	Led			B		0		276		3
//	Santer		B		3		1013	4


//	Task     State  Priority    Stack	#
//	************************************************
//	Shell		R		2		1468	1
//	IDLE		R		0		82		5
//	Serial2		B		2		470		2
//	Led			B		0		2044	3
//	Santer		B		3		1009	4

// 	316375


#endif

