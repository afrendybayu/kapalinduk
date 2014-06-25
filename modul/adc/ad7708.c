
#include "ad7708.h"
#include "monita.h"

#ifdef PAKAI_ADC_7708


extern volatile struct t_st_hw st_hw;
extern struct t_adc adc;

//unsigned short cek_reg_adc(unsigned char reg)	{
unsigned char cek_reg_adc(unsigned char reg)	{
	unsigned char cmd, xx, yy;
	
	cmd = m_read | reg;
	SSP1Send(cmd, 0);
	yy = SSP1Receive();
	SSP1Send(cmd, 0);
	xx = SSP1Receive();
	
	return xx;
	//return (unsigned short) ( (xx << 8) | yy);
}

//unsigned short set_reg_adc(unsigned char reg, unsigned char data)	{
unsigned char set_reg_adc(unsigned char reg, unsigned char data)	{
	unsigned char cmd, resp;
	
	cmd = m_write | reg;
	SSP1Send(cmd, 1);
	SSP1Receive();
	SSP1Send(data, 1);
	SSP1Receive();
	resp = cek_reg_adc(reg);
	
	return resp;
}

unsigned char setup_ad7708()	{
	unsigned char idadc, adaADC=0, ch, cadc;
	short jj, ww=0, gg=0;
	//uprintf("__%s masuk\r\n", __FUNCTION__);
	idadc = cek_adc_idx();		// cek
	vTaskDelay(1);
	ch = idadc>>4;
	//qsprintf("idadc : %02x >> ", idadc);

	if ( (ch==0x5) || (ch==0x4) )	{
		adaADC = 1;
	} else {
		return 0;
	}
	
	set_adc_ioconx(0x00);		// semua io port as input
	//uprintf("SET ioc : %02x\r\n", 0x00);
	//uprintf("ioc  : %04x \r\n", cek_adc_ioconx());
	vTaskDelay(1);
	set_adc_modex(0x01);		// mode idle
	//uprintf("SET mode : %02x\r\n", 0x01);
	//uprintf("mode : %04x \r\n", cek_adc_modex());
	adaADC = 2;
	uprintf("\r\n\r\nKalibrasi %d kanal ADC %s, id: %02x\r\n", JML_KANAL_ADC, (ch==5)?"AD7708":"AD7718", idadc );
	for (ch=0; ch<JML_KANAL_ADC; ch++)	{		// JML_KANAL_ADC
		uprintf("  Kanal %2d : ", ch+1);
		if (ch>=8)
			cadc = ((ch+6) << 4) | unipolar | rADC2560;		// REFIN 0-2.56V
		else
			cadc = (ch << 4) | unipolar | rADC2560;			// REFIN 0-2.56V
		set_adc_kontrolx(cadc);
		vTaskDelay(1);
		set_adc_modex(CHCON_10 | M_ADC_IZS);				// ena CHOP, CHCON:10ch
		vTaskDelay(1);
		jj = 0;
		do {
			jj++;
			vTaskDelay(10);
			//uprintf("mode : %04x \r\n", cek_adc_modex());
			cadc = cek_adc_modex() & M_ADC_IDLE;
			if (jj>100)		{
				adaADC = 3;
				break;
			}
		} while (cadc!=M_ADC_IDLE);
		
		if (adaADC==3)	{
			uprintf("GAGAL Kalibrasi [%d].\r\n", jj);
			gg++;
		} else {
			uprintf("OK : %d\r\n", jj);
			ww++;
		}
	}
	
	if (ww==JML_KANAL_ADC)	{
		uprintf("KALIBRASI KOMPLIT SUKSES %d ch\r\n", ww);
		adaADC = 4;
	} else {
		uprintf(" %d KANAL KALIBRASI ERROR.\r\n", gg);
		return adaADC;
	}
	
	//set_adc_filterx(RATE_FILTER_13);
	set_adc_filterx(RATE_FILTER_33);
	uprintf("ADC iocon 0x%02x; adccon 0x%02x; mode 0x%02x\r\n", 	\
		cek_adc_ioconx(), cek_adc_kontrolx(), cek_adc_modex());
	set_adc_ioconx(0x00);
	vTaskDelay(1);
	set_adc_modex(CHCON_10 | M_ADC_CONT_CONV);		// continues conversion
	
	adaADC = 5;
	st_hw.adc = 1;
	
	uprintf("Mulai koleksi data ADC .....0x%02x\r\n", cek_adc_modex());
	init_ad7708();
	
	return adaADC;
}

unsigned char koleksi_adc7708()	{
	adc.cur_kanal++;
	//if (adc.cur_kanal>(JML_KANAL_ADC-1)) adc.cur_kanal = 0;
	if (adc.cur_kanal>JML_KANAL_ADC) adc.cur_kanal = 0;
	//uprintf("%s ambil adc: %d\r\n", __FUNCTION__, adc.cur_kanal);
	
	//unsigned ch = (adc.cur_kanal << 4);
	unsigned char cadc;
	
	
	if (adc.cur_kanal>=8)
		cadc = ((adc.cur_kanal+6) << 4) | unipolar | rADC2560;		// REFIN 0-2.56V
	else
		cadc = (adc.cur_kanal << 4) | unipolar | rADC2560;			// REFIN 0-2.56V
	
	set_adc_kontrolx(cadc);
	//uprintf("adc kontrol : 0x%02x\r\n", cek_adc_kontrolx());
	return 1;
}

unsigned char ada_data_ad7708()	{
	adc.count++;
	adc.ambil = 1;
}

//int ambil_data_ad7708(uchar *data, int length)	{
int ambil_data_ad7708()	{
	char data[3];
	uchr st = cek_adc_statusx();
	if (cek_reg_adc(reg_status) & ADC_RDY)	{
		st_hw.adc_c++;
		cek_xreg_adc(reg_data, data, 3);
		adc.data[adc.cur_kanal] = (unsigned short) ((data[1] << 8) | data[2]);
		//qsprintf("data adc[%d]: %8d 0x%04x %02x %02x %02x\r\n", \
			adc.cur_kanal+1, adc.data[adc.cur_kanal], adc.data[adc.cur_kanal], data[0], data[1], data[2]);
		adc.ambil = 0;
		koleksi_adc7708();
		adc.count++;
		return 2;
	}
	return 0;
}

void init_ad7708()	{
	adc.count = 0;
	adc.cur_kanal = 1;
	adc.ambil = 0;
}

void set_xreg_adc(unsigned char reg, uchr *data, int length)	{
	int tt;
	uchr cmd = m_write | reg;
	SSP1Send(cmd, 1);
	SSP1Receive();
	for(tt=0; tt<(length-1); tt++)	{
		SSP1Send(*data, 1);
		SSP1Receive();
		data++;
	}
}

void cek_xreg_adc(unsigned char reg, uchr *data, int length)	{
	int tt;
	uchr cmd = m_read | reg;
	SSP1Send(cmd, 1);
	*data = SSP1Receive();
	for(tt=1; tt<length; tt++)	{
		SSP1Send(cmd, 1);
		data++;
		*data = SSP1Receive();
	}
}


void cek_adc_gain(void)	{
	
	unsigned char cmd;
	unsigned char xx;
	unsigned char isi[5];
	
	//cmd = m_read | reg_gain;
	cmd = m_read | reg_offset;
	SSP1Send(cmd, 1);
	isi[0] = SSP1Receive();
	SSP1Send(cmd, 1);
	isi[1] = SSP1Receive();
	SSP1Send(cmd, 1);
	isi[2] = SSP1Receive();
	//SSP1Send(cmd, 1);
	//isi[3] = SSP1Receive();
	//SSP1Terima(2);
	qsprintf("ofset adc: %02x %02x %02x\r\n", isi[2], isi[1], isi[0]);
	//qsprintf("ofset adc: %02x %02x %02x %02x\r\n", isi[3], isi[2], isi[1], isi[0]);
	//return xx;
}

#endif
