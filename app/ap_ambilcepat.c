
#include "FreeRTOS.h"
#include "task.h"
#include "ap_ambilcepat.h"
#include "monita.h"

#ifdef PAKAI_ADC_7708
#include "adc/ad7708.h"
#endif

#include <time.h>

xTaskHandle *hdl_cepat;
extern volatile struct t_st_hw st_hw;

void init_ambilCepatTasks( )		{
	xTaskCreate( sedotDataTask, ( signed portCHAR * ) "Santer", configMINIMAL_STACK_SIZE*ST_SANTER, NULL, \
		tskIDLE_PRIORITY+3, ( xTaskHandle * ) &hdl_cepat );
}

static portTASK_FUNCTION( sedotDataTask, pvParameters )		{
int ch;

	/* Just to stop compiler warnings. */
	( void ) pvParameters;
	vTaskDelay(200);
	//uprintf("  task : %s\r\n", __FUNCTION__);
	
	reset_konter();
	
	load_data_rtc();

	#ifdef PAKAI_SDCARD
		//st_hw.sdc = 0;
		//disk_initialize(SDC);
		#if 0
		if (cek_ins_sdc())	{
			disk_initialize(SDC);
			//ch = init_sdc();
			if (ch)		uprintf("___init SD ERROR !!___\r\n");
		} else {
			uprintf("___TIDAK ada SD Card !____\r\n");
		}
		#endif
		//vTaskDelay(10); ---> malah error tidak dikenali
	#endif
	
	#ifdef PAKAI_ADC_7708
		st_hw.adc = 0;
		#ifdef PAKAI_SDCARD
		vTaskDelay(700);
		#endif
		if (setup_ad7708()==0)
			uprintf("___ADC TIDAK dikenali !____\r\n");
	#endif
	


	
	
	st_hw.init++;
	int loopac=0, ff=0;
	struct tm a;
	
	int ww=0;
	
	gpio_int_init();
	int bunga;
	bunga = 1000/JUM_DIGITAL;
	
	for( ;; )	{
		

		#ifdef PAKAI_ADC_7708
			//if (adc.ambil == 1)	{
				if (cek_adc_statusx() & ADC_RDY)	{	//	0x81 & 0x80
					ambil_data_ad7708();
				}
			//}
		
			#if 0
			if (cek_adc_statusx() & ADC_RDY)	{	//	0x81 & 0x80
				ambil_data_ad7708();
			}
			#endif
		#endif
		
		vTaskDelay(1);
		loopac++;
		if (loopac>=bunga)	{ 
			hitung_rpm();
			cek_input_onoff();
			loopac = 0;
			#if 1
			ff++;
			if (ff>JUM_DIGITAL)	{
				data_frek_rpm();
				//uprintf(" waktu: %ld, fl: %d, on: %ld, rh: %ld, data_f[2]: %.0f\r\n", now_to_time(1, a), \
				//	konter.t_konter[2].rh_flag, konter.t_konter[2].rh_on, konter.t_konter[2].rh, data_f[2]);
				//uprintf("===> data rpm[1]: %.2f, rpm[2] : %.2f !!\r\n", data_f[0], data_f[1]);
				ff = 0;
				#if 0
				if (w==0)	{
					set_relay(1);
				} else {
					unset_relay(1);
				}
				ww=1-ww;
				#endif
				//printf("hit: %d\r\n", konter.t_konter[3].hit);
			}
			#endif
		}
	}
}
