
#ifndef __M_ADC__
#define __M_ADC__

#include "FreeRTOS.h"
#include "lpc23xx.h"
#include "hardware.h"
#include "spi_ssp.h"
#include "monita.h"

#ifdef PAKAI_ADC_7708

#define JML_KANAL_ADC		10

#define m_read				0x40
#define m_write				0x00
#define reg_status			0x00
#define reg_mode			0x01
#define reg_control			0x02
#define reg_filter			0x03
#define reg_data			0x04
#define reg_offset			0x05
#define reg_gain			0x06
#define reg_iocon			0x07
#define reg_id				0x0F
#define reg_com				0x00

#define reg_test1_adc		0x0C
#define reg_test2_adc		0x0D

#define unipolar			0x08
#define rADC20				0x00
#define rADC40				0x01
#define rADC80				0x02
#define rADC160				0x03
#define rADC320				0x04
#define rADC640				0x05
#define rADC1280			0x06
#define rADC2560			0x07

#define CHCON_10			BIT(4)

#define ADC_RDY				BIT(7)
#define ADC_CAL				BIT(5)
#define ADC_ERR				BIT(3)
#define ADC_LOCK			BIT(0)

#define M_ADC_PWRDOWN		0x00
#define M_ADC_IDLE			0x01
#define M_ADC_SINGLE_CONV	0x02
#define M_ADC_CONT_CONV		0x03
#define M_ADC_IZS			0x04
#define M_ADC_IFS			0x05
#define M_ADC_SZS			0x06
#define M_ADC_SFS			0x07

#define RATE_FILTER_13		0x68
#define RATE_FILTER_33		0x28

#define faktor_pengali_420	2.5

#define cek_adc_idx()		cek_reg_adc(reg_id)
#define cek_adc_modex()		cek_reg_adc(reg_mode)
#define cek_adc_statusx()	cek_reg_adc(reg_status)
#define cek_adc_kontrolx()	cek_reg_adc(reg_control)
#define cek_adc_ioconx()	cek_reg_adc(reg_iocon)
#define cek_adc_filterx()	cek_reg_adc(reg_filter)

#define set_adc_modex(d)	set_reg_adc(reg_mode, d)
#define set_adc_statusx(d)	set_reg_adc(reg_status, d)
#define set_adc_kontrolx(d)	set_reg_adc(reg_control, d)
#define set_adc_ioconx(d)	set_reg_adc(reg_iocon, d)
#define set_adc_filterx(d)	set_reg_adc(reg_filter, d)



//unsigned short cek_reg_adc(unsigned char reg);
unsigned char cek_reg_adc(unsigned char reg);
//unsigned short set_reg_adc(unsigned char reg, unsigned char data);
unsigned char set_reg_adc(unsigned char reg, unsigned char data);

unsigned char setup_ad7708();
void init_ad7708();
unsigned char koleksi_adc7708();
void cek_adc_gain(void);
void set_xreg_adc(unsigned char reg, uchr *data, int length);
void cek_xreg_adc(unsigned char reg, uchr *data, int length);
int ambil_data_ad7708();
unsigned char ada_data_ad7708();

#endif

#endif
