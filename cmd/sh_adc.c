
#include "FreeRTOS.h"
#include "task.h"
//#include "serial/tinysh.h"
//#include "sh_serial.h"
#include "sh_adc.h"
#include "adc/ad7708.h"
#include "monita.h"

#ifdef PAKAI_ADC_7708

extern volatile float data_f[];
extern struct t_adc adc;

void cek_adc(void)	{
	char i;
	uprintf(" ADC count = %d\r\n", adc.count);
	uprintf(" Ch | Kuantisasi |  Tegangan  |   Data   |\r\n********************************\r\n");
	for (i=0; i<JML_KANAL_ADC; i++ ) {
		uprintf(" %2d | %10d | %7.5f V | %.2f V\r\n", \
			i+1, adc.data[i], (float) (adc.data[i] * faktor_pengali_420 / 0xffff), data_f[JUM_DIGITAL+i]);
	}
}

#endif
