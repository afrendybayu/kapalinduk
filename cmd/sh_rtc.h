
#ifndef __SH_RTC__
#define __SH_RTC__

#include "tinysh/tinysh.h"

#ifdef PAKAI_RTC

void set_date_kitab();
void set_date(int argc, char **argv);
void init_RTC_sh();
void get_cal();
void cek_flag_RTC_sh();
void set_date_kitab();
void hitung_uptime();
void set_rtc_mem(int argc, char ** argv);
void cek_rtc_mem(int argc, char ** argv);

//static tinysh_cmd_t cek_irq_rtc_cmd={0,"cek_irqrtc","cek status IRQ rtc", "help default ",cek_rtc_irq_sh,0,0,0};
//static tinysh_cmd_t set_irq_rtcc_cmd={0,"set_irqrtc","set mode power", "help default ",set_rtc_counter_irq_sh,0,0,0};

static tinysh_cmd_t set_date_cmd={0,"set_date","Mengeset waktu","thn bulan tgl jam menit", set_date,0,0,0};	
static tinysh_cmd_t cek_flag_rtc_cmd={0,"frtc","","",  cek_flag_RTC_sh,0,0,0};
static tinysh_cmd_t init_rtc_cmd={0,"irtc","","",  init_RTC_sh,0,0,0};
static tinysh_cmd_t kalender_rtc_cmd={0,"kalender","","",  get_cal,0,0,0};
static tinysh_cmd_t uptime_cmd={0,"uptime","","",  hitung_uptime,0,0,0};
static tinysh_cmd_t set_rtc_mem_cmd={0,"set_mem","","",  set_rtc_mem,0,0,0};
static tinysh_cmd_t cek_rtc_mem_cmd={0,"cek_mem","","",  cek_rtc_mem,0,0,0};

#endif

#endif

