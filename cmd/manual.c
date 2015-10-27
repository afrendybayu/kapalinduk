
#ifndef __KITAB__
#define __KITAB__

#include "FreeRTOS.h"
#include "manual.h"
#include "monita.h"

void sumber_kitab()		{
	printf(" Perintah untuk mengubah konfig sumber !\r\n");
}

void env_kitab()	{
	printf(" Perintah untuk mengubah konfig modul !\r\n");
	printf(" 1. set_env help|default\r\n");
	printf("    help    : printout keterangan ini\r\n");
	printf("    default : memberikan default setting data\r\n");
	printf("\r\n");
	
	printf(" 2. set_env [argumen1] [argumen2]\r\n");
	printf("    set_env [ipaddr|gateway|server|nama|SN] [nilainya]\r\n");
	printf("    argumen1: ipaddr, gateway, server, nama, SN, file\r\n");
	printf("\r\n");
	printf(" Setting environment utama untuk board / modul\r\n");
	//garis_bawah();
	printf(" argument : help, default, ipaddr, gateway, nama\r\n");
	printf("   help   : printout keterangan ini\r\n");
	printf("   default : load default environment setting dari daun biru\r\n");
	printf(" \r\n");
	printf("   ipaddr : memberikan alamat IP\r\n");
	printf("     misalnya $ set_env ipaddr 192.168.1.124\r\n");
	printf(" \r\n");
	printf("   gateway : memberikan alamat IP gateway\r\n");
	printf("     misalnya $ set_env gateway 192.168.1.13\r\n");
	printf(" \r\n");
	printf("   nama : memberikan nama pada board\r\n");
	printf("     misalnya $ set_env nama BOARD_RTD_MAK_#3\r\n");
	printf(" \r\n");
	
	#ifdef PAKAI_WEBCLIENT
	printf("   webclient : aktivasi pengiriman data via webclient\r\n");
	printf("     misalnya $ set_env webclient [0|mati|1|aktif]\r\n");
	printf(" \r\n");
	printf("   webclientip : IP server untuk penyimpanan data\r\n");
	printf("     misalnya $ set_env webclientip 192.168.1.99\r\n");
	printf(" \r\n");
	printf("   file : tujuan file pengiriman data via webclient\r\n");
	printf("     misalnya $ set_env file /monita3/monita_loket.php\r\n");
	printf(" \r\n");
	printf("   burst : metode pengiriman data via webclient\r\n");
	printf("     mode burst sebaiknya digunakan jika mengirim beberapa data titik ukur\r\n");
	printf("	 dari beberapa sumber modul. Jml total data terkirim <20 titik.");
	printf("     misalnya $ set_env burst [0|mati|1|aktif]\r\n");
	printf(" \r\n");
	#endif
	
	#ifdef UNTUK_MONITA_KAPAL
	printf("   periode : periode pengiriman data (dalam detik)\r\n");
	printf("     misalnya $ set_env periode 10\r\n");
	printf(" \r\n");
	printf("   serclient : Aktivasi pengiriman data serial 2\r\n");
	printf("     misalnya $ set_env serclient [0|mati|1|aktif]\r\n");
	printf(" \r\n");
	printf("   jedareset : jeda antara reset flowmeter TFX dengan RPM 0\r\n");
	printf("     misalnya $ set_env jedareset 60 (dalam detik)\r\n");
	printf(" \r\n");
	#endif
}

void data_kitab()	{
	printf(" Perintah untuk mengubah konfig data !\r\n");
	printf(" 1. set_data help/default\r\n");
	printf("    help    : printout keterangan ini\r\n");
	printf("    default : memberikan default setting data\r\n");
	printf("\r\n");
	
	printf(" 2. set_data x [opt1] [opt2]\r\n");
	printf("    set_data x [nama|satuan|alarm|alarmLL|alarmL|alarmH|alarmHH] [nilainya]\r\n");
	printf("    x    : nomer data\r\n");
	printf("    opt1 : nama, set/aktif, desc/ket\r\n");
	printf("\r\n");
	
	printf("    [opt1]\r\n");				
	printf("    nama     : memberikan nama data yang akan ditampilkan\r\n");
	printf("    misalnya : $ set_data 2 nama GMT_#4\r\n");
	printf("    artinya memberikan nama data 1 dengan GMT_#4\r\n");
	printf("\r\n");
	
	printf("    alarm    : memberikan setting batasan alarm data\r\n");
	printf("    misalnya : $ set_data 2 alarmH 84.3\r\n");
	printf("    artinya memberikan setting batasan alarm tinggi data 2 dengan nilai 84.3\r\n");
	printf("    misalnya : $ set_data 5 alarmL 1.25\r\n");
	printf("    artinya memberikan setting batasan alarm rendah data 5 dengan nilai 1.25\r\n");
	printf("\r\n");
	
	printf("    relay : mengaktif/nonaktifkan relay pada kanal tertentu\r\n");
	printf("    misalnya  : $ set_data 4 alarm [1|2|3|aktif|hidup|atas|bawah|semua] 7 \r\n");
	printf("    artinya mengaktifkan relay untuk data ke 4 pada kanal 7\r\n");
	printf("    misalnya  : $ set_group 8 alarm [0|mati] 2\r\n");
	printf("    artinya me-nonaktifkan relay untuk data ke 8 pada kanal 2\r\n");
	printf("\r\n");
}

void kanal_kitab()	{
	printf(" Perintah untuk mengubah konfig kanal !\r\n");
}

void info_kanal()	{
	printf("Setting Nomor Status input kanal\r\n");
	printf("CMD : set_kanal [nokanal] status [nostatus]\r\n");
	printf("   %4d : sRPM\r\n", sRPM);
	printf("   %4d : sONOFF\r\n", sONOFF);
	printf("   %4d : sPUSHBUTTON\r\n", sPUSHBUTTON);
	//printf("   %4d : sFLOW1\r\n", sFLOW1);
	printf("   %4d : sFLOWx\r\n", sFLOWx);
	//printf("   %4d : sFLOW2\r\n", sFLOW2);
	printf("   %4d : sRPM_RH\r\n", sRPM_RH);
	printf("   %4d : sRUNNING_HOURS\r\n", sRUNNING_HOURS);
	printf("   %4d : sONOFF_RH\r\n", sONOFF_RH);
	printf("   %4d : ANALOG MONITA\r\n", 250);
	printf("   %4d : ANALOG RUNNINGHOUR\r\n", 999);
	printf("   %4d : NO STATUS\r\n", sNONE);
}

void file_kitab()	{
	
}

void rtc_kitab(int i)	{
	printf(" Perintah untuk mengubah nilai memori RTC !\r\n");
}

#endif
