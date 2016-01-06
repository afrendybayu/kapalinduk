
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
void set_modem_echo()	{
	printf("set Echo Modem On\r\n");
	vTaskDelay(100);
	flagModem = ECHO_OFF;
	perintah_modem(flagModem);
}

