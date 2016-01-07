
#include "FreeRTOS.h"
#include "task.h"

#include "sh_modem.h"
#include "ap_serial1.h"




void cek_modem()	{
	printf(" Cek AT Modem \r\n");
	vTaskDelay(100);
	flagModem = CEK_AT;
	perintah_modem(flagModem);
	}
	
void set_modem_echo(int argc , char **argv)	{
	if (strcmp(argv[1],"1",1)==0)
	{printf("set Echo Modem On\r\n");
	vTaskDelay(100);
	flagModem = ECHO_ON;
		}
	else if (strcmp(argv[1],"0",1)==0)
	{printf("set Echo Modem Off\r\n");
	vTaskDelay(100);
	flagModem = ECHO_OFF;
	}
	
	perintah_modem(flagModem);
}

void set_modem_opr(int argc , char **argv)	{
	if (strcmp(argv[1],"TELKOMSEL",9)==0)
	{printf("set operator menjadi %s\r\n",argv[1]);
	vTaskDelay(100);
	flagModem =OPR_TSEL;
		}
	else if (strcmp(argv[1],"0",1)==0)
	{printf("set Echo Modem Off\r\n");
	vTaskDelay(100);
	flagModem = ECHO_OFF;
	}
	
	perintah_modem(flagModem);
}
