
// Afrendy Bayu
// Depok, 13 Agustus 2013
// 

#ifdef PAKAI_RELAY

#include "FreeRTOS.h"
#include "task.h"
#include "monita.h"

#include "sh_relay.h"
#include "sh_utils.h"
#include "manual.h"


char cek_relay(int argc, char **argv)	{
	if (argc>2)		{
		relay_kitab();
	}
	
	int i;
	printf("\r\n");
	int tRelay = JML_SUMBER * PER_SUMBER;
	if (argc==1)	{
		for(i=0; i<JML_RELAY; i++)		{
			qsprintf("  Relay-%d : %d - %s\r\n", i+1, (int) data_f[tRelay+i], (int) (data_f[tRelay+i]) ?"Aktif","Mati");
		}
	}
	
	if (argc==2)	{
		i = cek_nomor_valid(argv[1], JML_RELAY);		
		if ( (i==TIDAK_VALID) || (i==0) )	{
			printf("  no sumber TIDAK VALID\r\n");
			return 1;
		}
		i--;
		qsprintf("  Relay-%d : %d - %s\r\n", i+1, (int) data_f[tRelay+i], (int) (data_f[tRelay+i]) ?"Aktif","Mati");
	}
	return 0;
}

char set_relay(int argc, char **argv)	{
	if (argc!=3)	{
		printf("\r\n  --> Perintah Salah.\r\n");
		relay_kitab();
		return 1;
	}
	
	int no = cek_nomor_valid(argv[1], JML_RELAY);		
	if ( (no==TIDAK_VALID) || (no==0) )	{
		printf("  no sumber TIDAK VALID\r\n");
		return 1;
	}
	no--;
	int tRelay = JML_SUMBER * PER_SUMBER;
	
	if ( (strcmp(argv[2], "aktif")==0) || (argv[2][0]=='1') )	{
		if 		 (no==0)	sRelay1();
		else if (no==1)	sRelay2();
		else if (no==2)	sRelay3();
		else if (no==3)	sRelay4();
		else if (no==4)	sRelay5();
		else if (no==5)	sRelay6();
		else if (no==6)	sRelay7();
		else if (no==7)	sRelay8();
		data_f[tRelay+no] = 1;
	}
	else if ( (strcmp(argv[2], "mati")==0) || (argv[2][0]=='0') )	{
		if 		 (no==0)	cRelay1();
		else if (no==1)	cRelay2();
		else if (no==2)	cRelay3();
		else if (no==3)	cRelay4();
		else if (no==4)	cRelay5();
		else if (no==5)	cRelay6();
		else if (no==6)	cRelay7();
		else if (no==7)	cRelay8();
		data_f[tRelay+no] = 0;
	}
	qsprintf("  Relay-%d : %d - %s\r\n", i+1, (int) data_f[tRelay+i], (int) (data_f[tRelay+i]) ?"Aktif","Mati");
}



#endif
