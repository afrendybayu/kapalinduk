
// Afrendy Bayu
// Depok, 29 Juli 2013
// 

#include "FreeRTOS.h"
#include "task.h"
#include "monita.h"

#include "sh_env.h"
#include "sh_utils.h"
#include "manual.h"
#include "iap.h"

//extern struct t_env env;

void cek_env(int argc, char **argv)	{
	IAP_return_t iap_return;
	iap_return = iapReadBlankSector(SEKTOR_ENV, SEKTOR_ENV);
	if (iap_return.ReturnCode == CMD_SUCCESS)	{
		printf("  ERR: Sektor Kosong ! Lakukan: set_env default\r\n");
		return;
	}
	
	struct t_env *st_env;
	st_env = ALMT_ENV;
	
	uprintf("\r\n  Cek modul %s %s   \r\n  ******************************\r\n", BOARD_SANTER, BOARD_SANTER_versi);
	uprintf("  Nama Board : %s\r\n", st_env->nama_board);
	uprintf("  No Seri    : %s\r\n", st_env->SN);
	uprintf("  ID Modem : %s\n\r", st_env->id_modem);
	//uprintf("  Waktu Modem : %s\n\r", st_env->waktu_modem);
	uprintf("  Waktu Modem : %d\n\r", st_env->waktu_modem);
	//uprintf("  No Ajaib   : %02X:%02X\r\n", st_env->magic1, st_env->magic2);
	#ifdef PAKAI_ETH
	uprintf("  Konfigurasi Ethernet\r\n");
	uprintf("    Alamat IP: %d.%d.%d.%d\r\n", st_env->IP0, st_env->IP1, st_env->IP2, st_env->IP3);
	uprintf("    Gateway  : %d.%d.%d.%d\r\n", st_env->GW0, st_env->GW1, st_env->GW2, st_env->GW3);
	uprintf("    Server IP: %d.%d.%d.%d\r\n", st_env->wIP0, st_env->wIP1, st_env->wIP2, st_env->wIP3);
	uprintf("    File     : %s\r\n", st_env->berkas);
	uprintf("    Webclient: [%d] : %s\r\n", st_env->statusWebClient, (st_env->statusWebClient?"Aktif":"Mati"));
	#endif
	#ifdef MODBUS_RTU_SLAVE
	uprintf("  Konfigurasi Modbus RTU Slave\r\n");
	uprintf("    ID Device: %dd = %#02x\r\n", st_env->almtSlave, st_env->almtSlave);
	uprintf("    BaudRate : %d\r\n", PAKAI_SERIAL_2_P0);
	#endif
	#ifdef MODBUS_RTU_MASTER
	uprintf("  Konfigurasi Modbus RTU Master\r\n");
	uprintf("    ID Device: %dd = %#02x\r\n", st_env->almtMaster, st_env->almtMaster);
	uprintf("    BaudRate : %d\r\n", PAKAI_SERIAL_3_P4);
	#endif
	//uprintf("  Konfig Cron  : [%d] : %s\r\n", st_env->statusCron, (st_env->statusCron?"Aktif":"Mati"));
	uprintf("  Konfig Debug1: %d\r\n", st_env->prioDebug);
	uprintf("  Konfig Debug2: %d\r\n", st_env->prioDebug2);
	uprintf("  Memory space: %d\r\n", xPortGetFreeHeapSize());
	
	uprintf("  Moving average: %d\n\r",st_env->n_mavg);
	
	uprintf("  Fuel Density: %d\r\n", st_env->fuel_den);
	uprintf("  Jumlah Flowmeter: %d|%d\r\n", st_env->nFL,st_env->jumFlow);
	
}



char set_env(int argc, char **argv)	{
	if (argc>3 || argc==1)		env_kitab();
	if (argc==2)	{
		printf("\r\n");
		if (strcmp(argv[1], "help") == 0)	{
			env_kitab();
			return 0;
		}
		else if (strcmp(argv[1], "default") == 0)	{
			printf("  set env dengan konfig default !\r\n");
			set_env_default();
			return 0;
		}
		return 1;
	}
	
	printf("\r\n");
	struct t_env *st_env;
	st_env = pvPortMalloc( sizeof (struct t_env) );
	if (st_env == NULL)	{
		printf(" %s(): ERR allok memory gagal !\r\n", __FUNCTION__);
		vPortFree(st_env);
		return 2;
	}
	printf("  %s(): Mallok @ %X\r\n", __FUNCTION__, st_env);
	memcpy((char *) st_env, (char *) ALMT_ENV, (sizeof (struct t_env)));
	
	if (argc==3)	{
		
		if (strcmp(argv[1], "SN") == 0)	{
			printf(" set no seri \r\n");
			if (strlen(argv[2]) > sizeof (st_env->SN))		{
				printf("SN terlalu panjang !");
				return 3;	
			}
			else	{
				sprintf(st_env->SN, "%s", argv[2]);
				printf(" SN : %s\n", st_env->SN);
			}
		}
		#ifdef PAKAI_ETH
		else if (strcmp(argv[1], "ipaddr") == 0)  	{
			printf(" set Alamat IP ");  
			
			uint ret_ip = baca_ip( argv[2] );
			if (ret_ip > 0)	{
				st_env->IP0 = (unsigned char)(ret_ip >> 24);
				st_env->IP1 = (unsigned char)(ret_ip >> 16);
				st_env->IP2 = (unsigned char)(ret_ip >> 8);
				st_env->IP3 = (unsigned char)(ret_ip);
				printf("%d.%d.%d.%d\r\n", st_env->IP0, st_env->IP1, st_env->IP2, st_env->IP3);
			} 
			else		return 4;
		}
		else if (strcmp(argv[1], "gateway") == 0)  	{
			printf(" set GATEWAY address ");  

			uint ret_ip = baca_ip( argv[2] );
			if (ret_ip > 0)	{
				st_env->GW0 = (unsigned char)(ret_ip >> 24);
				st_env->GW1 = (unsigned char)(ret_ip >> 16);
				st_env->GW2 = (unsigned char)(ret_ip >> 8);
				st_env->GW3 = (unsigned char)(ret_ip);
				printf("%d.%d.%d.%d\r\n", st_env->GW0, st_env->GW1, st_env->GW2, st_env->GW3);
			} 
			else		return 4;
		}
		else if (strcmp(argv[1], "server") == 0)  	{
			printf(" set Server Tujuan address ");  

			uint ret_ip = baca_ip( argv[2] );
			if (ret_ip > 0)	{
				st_env->wIP0 = (unsigned char)(ret_ip >> 24);
				st_env->wIP1 = (unsigned char)(ret_ip >> 16);
				st_env->wIP2 = (unsigned char)(ret_ip >> 8);
				st_env->wIP3 = (unsigned char)(ret_ip);
				printf("%d.%d.%d.%d\r\n", st_env->wIP0, st_env->wIP1, st_env->wIP2, st_env->wIP3);
			} 
			else		return 5;
		}
		#endif
		else if (strcmp(argv[1], "nama") == 0)	{
			printf(" set nama board\r\n");

			if (strlen(argv[2]) > sizeof (st_env->nama_board))		{
				printf("Nama board terlalu panjang !");
				return 6;	
			}
			else	{
				sprintf(st_env->nama_board, "%s", argv[2]);
				printf(" Nama board : %s\r\n", st_env->nama_board);
			}
		}
		#ifdef MODBUS_RTU_SLAVE
		else if (strcmp(argv[1], "idslave") == 0)	{
			printf("  idSlave\r\n");
			st_env->almtSlave = atoi( argv[2] );
			printf("  Alamat ID Slave : %d\r\n", st_env->almtSlave);
			
		}
		#endif
		#ifdef MODBUS_RTU_MASTER
		else if (strcmp(argv[1], "idmaster") == 0)	{
			printf("  idMaster\r\n");
			st_env->almtMaster = atoi( argv[2] );
			printf("  Alamat ID Master : %d\r\n", st_env->almtMaster);
			
		}
		#endif
		else if (strcmp(argv[1], "debug1") == 0)	{
			printf("  Debug prio serial0\r\n");
			st_env->prioDebug = atoi( argv[2] );
			printf("  Nomor batas prioritas debug : %d\r\n", st_env->prioDebug);
			
		}
		else if (strcmp(argv[1], "debug2") == 0)	{
			printf("  Debug prio serial2\r\n");
			st_env->prioDebug2 = atoi( argv[2] );
			printf("  Nomor batas prioritas debug2 : %d\r\n", st_env->prioDebug2);
			
		}	
		else if (strcmp(argv[1], "density") == 0)	{
			printf("Set Density\n\r");
			st_env->fuel_den = atoi(argv[2]);
			printf("Fuel Density = %d",st_env->fuel_den);
			
		}
		else if (strcmp(argv[1], "flow") == 0)	{
			st_env->jumFlow = 1;
			printf("Set Jumlah kanal Flowmeter\n\r");
			unsigned int cnt;
			//cnt = atoi(argv[2]);
			for (cnt = 0; cnt<((atoi(argv[2]))-1); cnt++)st_env->jumFlow = (st_env->jumFlow<<1)+1;
			printf("Jumlah Kanal Flowmeter = %d",(atoi(argv[2])));
			st_env->nFL = atoi(argv[2]);
		}
	}
	
	//simpan_struct_block_rom(SEKTOR_ENV, ENV, 0, (char *) st_env);
	simpan_st_rom(SEKTOR_ENV, ENV, 1, (unsigned short *) st_env, 0);
	vPortFree (st_env);
	
	printf("\r\n");
	return 0;
}

void set_env_default()		{
	int i;
	
	struct t_env *st_env;
	st_env = pvPortMalloc( sizeof (struct t_env) );
	
	if (st_env==NULL)	{
		printf("  GAGAL alokmem !");
		vPortFree (st_env);
		return;
	}
	//memcpy((char *) st_env, (char *) ALMT_ENV, (sizeof (struct t_env)));
	printf("  %s(): Mallok @ %X\r\n", __FUNCTION__, st_env);
	strcpy(st_env->nama_board, BOARD_SANTER);
	st_env->IP0 = 192;
	st_env->IP1 = 168;
	st_env->IP2 = 1;
	st_env->IP3 = 250;
	st_env->GW0 = 192;
	st_env->GW1 = 168;
	st_env->GW2 = 1;
	st_env->GW3 = 1;
	
	for (i=0; i<(JML_KANAL); i++)	{
		st_env->kalib[i].m = 1;
		st_env->kalib[i].C = 0;
		st_env->kalib[i].status = sRPM;
		//strcpy(env.kalib[i].ket, "----");
		//printf("%d", sRPM);
	}
	
	for (i=0; i<(JML_KANAL); i++)	{
		st_env->kalib[i+JML_KANAL].m = 1;
		st_env->kalib[i+JML_KANAL].C = 0;
		st_env->kalib[i+JML_KANAL].status = sADC_7708;
		//strcpy(env.kalib[i].ket, "----");
		//printf("%d", sADC_7708);
	}
	
	
	st_env->magic1 = 0x01;
	st_env->magic2 = 0x03;
	st_env->mmc_serial = 0;
	strcpy(st_env->SN, "STR.kalender");

	strcpy(st_env->berkas, "/monita3/ml.php");
	st_env->statusWebClient = 0;
	st_env->banyak_sumber = 5;
	st_env->burst = 0;
	st_env->wIP0 = 192;
	st_env->wIP1 = 168;
	st_env->wIP2 = 1;
	st_env->wIP3 = 99;
	st_env->k1 = 0;
	st_env->k2 = 0;
	st_env->uRTC = 0;
	st_env->statusSerClient = 0;
	st_env->intKirim = 10;
	st_env->intReset = 30;
	st_env->intTole = 10;
	st_env->netTot = 10;
	strcpy(st_env->passwd, "monita");
	strcpy(st_env->madein, "Afrendy Bayu");
	strcpy(st_env->nohp, "08111136880");
	st_env->statusCron = 0;
	st_env->almtSlave = 1;
	st_env->statusSlave = 0;
	st_env->prioDebug  = 10;
	st_env->prioDebug2 = 20;
	st_env->jmlfile = 0;
	st_env->almtMaster = 1;
	st_env->n_mavg = 0;
	st_env->n_move = 3;
	
	simpan_st_rom(SEKTOR_ENV, ENV, 0, (unsigned short *) st_env, 0);
	//simpan_struct_block_rom(SEKTOR_ENV, ENV, 1, (char *) &st_env);
	//simpan_struct_block_rom(SEKTOR_ENV, ENV, 1, &st_env);
	vPortFree (st_env);
}

