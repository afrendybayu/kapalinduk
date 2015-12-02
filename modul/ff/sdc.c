
/****************************
*		Afrendy Bayu
*   Depok, 16 April 2013
* http://www.k9spud.com/sdcard/
* http://www.ece.ucdavis.edu/vcl/vclpeople/jwwebb/measbd/ctrl_fpga/cdev/microsd_driver/html/microsd_8c-source.html
****************************/

#include "FreeRTOS.h"

#ifdef PAKAI_SDCARD


#include "sdc.h"
#include "monita.h"
#include "hardware.h"
#include "spi_ssp.h"
#include "ff/ff9b/src/ff.h"
#include "ff/ff9b/src/diskio.h"



#define DEBUG_SDC

extern volatile struct t_st_hw st_hw;

inline unsigned char cek_ins_sdc()	{
	//unsigned char a = ((FIO2PIN & BIT(INS_SDC)) >> INS_SDC);
	return PORT2_INPUT(INS_SDC);
}

void SD_kirimCmd(unsigned char* cmd)	{
	int i;
	for(i=0; i<6; i++)	{
		//uprintf("%02x ", cmd[i]);
		//SSP0Send(cmd[i], 1);
		SSP0Send(*cmd, 1);
		//uprintf("%02x ", *cmd);
		cmd++;
	}
}

unsigned char respon_SDcmd(unsigned char* rspn, int length)	{
	int i=0, j=0;
	for(i = 0; i < length; i++)		{
		SSP0Send(0xff, 1);
		rspn[i] = SSP0Receive();
		//uprintf("rspn: %02x\r\n", rspn[i]);
		//uprintf("%02x ", rspn[i]);
		if (rspn[i] != 0xff)	j++;
	}
	return j;
}

unsigned char SDC_ioctl (
	uchr cmd,		/* Control code */
	void *buff		/* Buffer to send/receive data block */
)	{
	
	DRESULT res;
	BYTE b, *ptr = buff, sdstat[64];
	DWORD resp[4], d, *dp, st, ed;
	static const DWORD au_size[] = {1, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 24576, 32768, 49152, 65536, 131072};
	
	
	switch (cmd) {
		case GET_SECTOR_COUNT :	/* Get number of sectors on the disk (DWORD) */
			if ((CardInfo[0] >> 6) == 1) {	/* SDC CSD v2.0 */
				d = CardInfo[9] + ((WORD)CardInfo[8] << 8) + ((DWORD)(CardInfo[7] & 63) << 16) + 1;
				*(DWORD*)buff = d << 10;
			} else {						/* MMC or SDC CSD v1.0 */
				b = (CardInfo[5] & 15) + ((CardInfo[10] & 128) >> 7) + ((CardInfo[9] & 3) << 1) + 2;
				d = (CardInfo[8] >> 6) + ((WORD)CardInfo[7] << 2) + ((WORD)(CardInfo[6] & 3) << 10) + 1;
				*(DWORD*)buff = d << (b - 9);
			}
			res = RES_OK;
			break;
		case GET_BLOCK_SIZE :	/* Get erase block size in unit of sectors (DWORD) */
			if (CardType & CT_SD2) {	/* SDC ver 2.00 */
				if (SDC_ioctl(MMC_GET_SDSTAT, sdstat)) break;
				*(DWORD*)buff = au_size[sdstat[10] >> 4];
			} else {					/* SDC ver 1.XX or MMC */
				if (CardType & CT_SD1)	/* SDC v1 */
					*(DWORD*)buff = (((CardInfo[10] & 63) << 1) + ((WORD)(CardInfo[11] & 128) >> 7) + 1) << ((CardInfo[13] >> 6) - 1);
				else					/* MMC */
					*(DWORD*)buff = ((WORD)((CardInfo[10] & 124) >> 2) + 1) * (((CardInfo[11] & 3) << 3) + ((CardInfo[11] & 224) >> 5) + 1);
			}
			res = RES_OK;
			break;
		case MMC_GET_SDSTAT :	/* Receive SD status as a data block (64 bytes) */
			if (CardType & CT_SDC) {	/* SDC */
				#if 0
				if (wait_ready(500)) {
					ready_reception(1, 64);				/* Ready to receive data blocks */
					if (send_cmd(ACMD13, 0, 1, resp)	/* Start to read */
							&& !(resp[0] & 0xC0580000)) {
						while ((XferWp == 0) && !(XferStat & 0xC));
						if (!(XferStat & 0xC)) {
							Copy_al2un(buff, DmaBuff[0], 64);
							res = RES_OK;
						}
					}
				}
				stop_transfer();					/* Close data path */
				#endif
			}
			break;
	}
}

void sdc_ket()		{
	float sdc_cap;
	
	// CID : Card Identification Register
	cInfo.manid = CardInfo[16+0];		 							// Manufacturer ID
	memcpy(cInfo.name, &CardInfo[16+3], 5);	 cInfo.name[5] = '\0';	// Product Name
	//memcpy(cInfo.appid, &cid[j+1], 2);						// OEM/Application ID
	cInfo.oem = (CardInfo[16+1] << 8) | CardInfo[16+2];
	cInfo.rev = CardInfo[16+8];									// Product Revision
	cInfo.year = 2000 + ((CardInfo[16+13] & 0x0f) << 4) | (CardInfo[16+14] >> 4);	// Product Date - year
	cInfo.month = (CardInfo[16+14] & 0x0f);					// Product Date - month
	cInfo.serial = (CardInfo[16+9] << 24) | (CardInfo[16+10] << 16) | (CardInfo[16+11] << 8) | CardInfo[16+12]; // Serial Number
	uprintf("CID --> MID: %d, Nama: %s, Rev%d, SN: 0x%08X, ManDate: %02d-%d, oem: 0x%04X\r\n", \
		cInfo.manid, cInfo.name, cInfo.rev, cInfo.serial, cInfo.month, cInfo.year, cInfo.oem);
		
	cInfo.speed = CardInfo[3];
	cInfo.nblock = 1 << (CardInfo[5] & 0x0F);
	if (cInfo.CardType & CT_SD2) {		// SDC ver 2
		cInfo.blocks = (((CardInfo[7] & 0x3F) << 16) | (CardInfo[8] << 8) | CardInfo[9]) + 1;
		//cInfo.sector = CardInfo[7] & 0x7F;
		sdc_cap =  cInfo.blocks * 512 * 1024;
	} else {							// SDC ver 1 & MMC
		unsigned int C_Size      = ((CardInfo[6] & 0x03) << 10) | (CardInfo[7] << 2) | ((CardInfo[8] >>6) & 0x03);
		unsigned int C_Size_Mult = ((CardInfo[9] & 0x03) << 1) | ((CardInfo[10] >> 7) & 0x01);
		unsigned int Read_Bl_Len = CardInfo[5] & 0x0f;
		cInfo.blocks  = ((C_Size + 1) * (1 << (C_Size_Mult + 2)) * (1 << Read_Bl_Len)) / 512;
	}

	uprintf("CSD --> Speed: 0x%02xh: %s MHz, CardType: 0x%02Xh, nBlock: %d, C_Size: %d, Total cap: %ld\r\n", \
		cInfo.speed, (cInfo.speed==0x32)?"25":"-", cInfo.CardType, cInfo.nblock, cInfo.blocks, (unsigned long) sdc_cap);	// 
	
	
}

uchr sdc_info()	{
	unsigned char status, j=0, k;
	unsigned char ocr[4];
	unsigned char cid[20];			// Card Identification Register
		
	status = Microsd_SendCmd( CMD58, 0x00000000 );		// OCR
	respon_SDcmd( ocr, R3_RESPONSE_SIZE );
	//flush_ssp_spi();
	memcpy(&CardInfo[32], &ocr[0], 4);
	
	status = Microsd_SendCmd( CID, 0x00000000 );		// CID
	respon_SDcmd( cid, 20 );
	while (cid[j] != START_TOKEN) {
		j++;
	}
	j++;
	//flush_ssp_spi();
	memcpy(&CardInfo[16], &cid[j], 16);
	//uprintf("CID: %02x ", cid[j-1]);	for(k=0; k<16; k++)		uprintf("%02x ", CardInfo[16+k]);	uprintf("\r\n");

	vTaskDelay(1);
	status = Microsd_SendCmd( CSD, 0x00000000 );		// CSD
	respon_SDcmd( cid, 20 );
	//uprintf("++++CSD: ");	for(k=0; k<20; k++)		uprintf("%02x ", cid[k]);	uprintf("\r\n");
	while (cid[j] != START_TOKEN) {
		j++;
	}
	j++;
	//flush_ssp_spi();
	memcpy(&CardInfo[0], &cid[j], 16);
	//uprintf("CSD: %02x ", cid[j-1]);	for(k=0; k<16; k++)		uprintf("%02x ", CardInfo[k]);	uprintf("\r\n");

	vTaskDelay(1);
	unsigned char CMD55_APP_CMD[] = {55,0x00,0x00,0x00,0x00,0xFF};
	status = SD_WriteCmd(CMD55_APP_CMD);
	
	
	if (status==0x00)	{
		do {
			j=0;
			vTaskDelay(1);
			status = Microsd_SendCmd( SCR, 0x00000000 );	// SCR
			respon_SDcmd( cid, 20 );
			//uprintf("===> SCR: ");	for(k=0; k<20; k++)		uprintf("%02x ", cid[k]);	uprintf("\r\n");
			while ( (cid[j] != START_TOKEN) && (j>19) ) {
				j++;
			}
			//printf("j: %d\r\n", j);
		} while (j==19);
		
		#if 0
		status = Microsd_SendCmd( SCR, 0x00000000 );	// SCR
		respon_SDcmd( cid, 20 );
		//uprintf("===> SCR: ");	for(k=0; k<20; k++)		uprintf("%02x ", cid[k]);	uprintf("\r\n");
		while (cid[j] != START_TOKEN) {
			j++;
		}
		j++;
		#endif
		//flush_ssp_spi();
		memcpy(&CardInfo[36], &cid[j], 4);
		//for(k=0; k<4; k++)		uprintf("%02x ", CardInfo[36+k]);	uprintf("\r\n");
	}

	vTaskDelay(10);
	sdc_ket();
	//printf("info selesai\r\n");
}

uchr sdc_init()	{
	int i;
	unsigned char status, ty, cmd;
	unsigned char strRsp[4];
	
	setup_spi_ssp0();
	init_ssp0(SCK_LAMA);

	for(status=0; status<10; status++)	{
		for(i=0; i<10000; i++);
		for(i=0; i<10000; i++);
	}
	vTaskDelay(50);
	flush_ssp_spi();
	
	// Wait for the SD Card to go into IDLE state
	i = 0;	status = 0xff;
	do	{
		unsigned char CMD0_GO_IDLE_STATE[] = { 0x00,0x00,0x00,0x00,0x00,0x95 };
		status = SD_WriteCmd(CMD0_GO_IDLE_STATE);
		vTaskDelay(1);
		//if (i++ > 25)	{
		if (i++ > 10000)	{
			return 1;
		}
	} while( status != 0x01 );
	#ifdef DEBUG_SDC
	//uprintf("---> CMD0_GO_IDLE_STATE status[%d] : %d\r\n", i, status);
	#endif
	flush_ssp_spi();
	
	unsigned int iRsp;
	Microsd_SendCmd( CMD8, 0x000001AA );
	status = respon_SDcmd( strRsp, R7_RESPONSE_SIZE );
	iRsp =  (strRsp[0] << 24) + (strRsp[1] << 16) + (strRsp[2] << 8) + strRsp[3];
	
	//uprintf("\r\nhasil respon : %04x : %02x %02x %02x %02x\r\n", iRsp, strRsp[0], strRsp[1], strRsp[2], strRsp[3]);
	
	unsigned char CMD55_APP_CMD[] = {55,0x00,0x00,0x00,0x00,0xFF};
	if ((iRsp & 0xFFF) == 0x1AA)	{
		//uprintf("masuk cek versi 2 !!\r\n");
		i=0;
		do	{
			if (i++ > 100)	return 2;
			vTaskDelay(5);
			//printf("---> cek_APP_CMD\r\n");
			if (SD_WriteCmd(CMD55_APP_CMD)==0x01)	{
				// SDHC & SDXC supported, sdxc power saving, use current signal voltage
				vTaskDelay(5);
				status = Microsd_SendCmd( CMD41, 0x40FF8000 );	// 4000000000
				
				respon_SDcmd( strRsp, R3_RESPONSE_SIZE );
				#ifdef DEBUG_SDC
				//uprintf("---> CMD41_SD_SEND_OP_COND status[%d] : %02x\r\n", i, status);
				//uprintf("respon v2 : %02x %02x %02x %02x\r\n", strRsp[0], strRsp[1], strRsp[2], strRsp[3]);
				#endif
			}
			//vTaskDelay(50);
			//printf("---> habis delay status: %02x\r\n", status);
		} while (status==0x01);
		
		//vTaskDelay(50);
		
		status = Microsd_SendCmd( CMD58, 0x00000000 );
		respon_SDcmd( strRsp, R3_RESPONSE_SIZE );
		
		//printf("---> CMD58 selesai\r\n");
		ty = (strRsp[0] & 0x40) ? CT_SD2|CT_BLOCK : CT_SD2;		// Check CCS bit in the OCR 0x04 | 0x08  : 0x04;
		cInfo.version = CARD_VERSD_20;
		//printf("---> memcpy ocr\r\n");
		vTaskDelay(5);
		
		memcpy(cInfo.ocr, &strRsp[0], 4);
		//uprintf("---> OCR[%d] tipe: %02x, respon : %02x %02x %02x %02x\r\n", i, ty, strRsp[0], strRsp[1], strRsp[2], strRsp[3]);
		// CCS 0: SDHC/SDXC     CCS 1: SDSC
		
	} else {
		uprintf("masuk cek versi: SDC Ver1 & MMC !!\r\n");
		if (SD_WriteCmd(CMD55_APP_CMD)==0x01)	{
			status = Microsd_SendCmd( CMD41, 0x00FF8000 );
			respon_SDcmd( strRsp, R3_RESPONSE_SIZE );
			
			#ifdef DEBUG_SDC
			uprintf("---> CMD41_SD_SEND_OP_COND status[%d] : %02x\r\n", i, status);
			#endif
			
			if (status == 0x01)		{		// SD v1
				ty = CT_SD1; cmd = CMD41;	cInfo.version = CARD_VERSD_10;
			} else {
				ty = CT_MMC; cmd = CMD1;	cInfo.version = CARD_VERMM_30;
			}
			i=0;
			do {
				if (i++ > 20)	return 3;
				vTaskDelay(10);
				if (cmd==CMD41)	{
					if (SD_WriteCmd(CMD55_APP_CMD)==0x01)	{
						// SDHC & SDXC supported, sdxc power saving, use current signal voltage
						status = Microsd_SendCmd( CMD41, 0x00FF8000 );	// 4000000000
						respon_SDcmd( strRsp, R3_RESPONSE_SIZE );
						//uprintf("respon v1 : %02x %02x %02x %02x\r\n", strRsp[0], strRsp[1], strRsp[2], strRsp[3]);
						
					}
				} else {
					status = Microsd_SendCmd( CMD41, 0x00FF8000 );	// 4000000000
					respon_SDcmd( strRsp, R3_RESPONSE_SIZE );
				}
				
			} while (status == 0x01);
			#ifdef DEBUG_SDC
				uprintf("---> CMD%d status[%d] : %02x\r\n", cmd, i, status);
			#endif
		}

	}

	flush_ssp_spi();
	vTaskDelay(5);
	init_ssp0(SCK_KILAT);
	st_hw.sdc = 1;
	vTaskDelay(5);
	cInfo.CardType = ty;
	sdc_info();	
	return 0;
}


unsigned char sdc_read(unsigned char *data, unsigned int almt, int len)	{
						// data hasil				sektor			jumlah
	unsigned short crc;
	unsigned short resp;
	unsigned char cmd, rspn;
	//unsigned char hsl[512];
	int a,b;
	
	if (len < 1 || len > 127) return 1;	/* Check parameter */
	//if (Stat & STA_NOINIT) return RES_NOTRDY;	/* Check drive status */
	
	cmd = (len > 1) ? CMD18 : CMD17;		// Transfer type: Single block or Multiple block //
	//cmd = CMD17;
	almt *= 512;
	rspn = Microsd_SendCmd( cmd, almt );
	//uprintf("CMD: %d, almt: %d, len: %d, status %02x\r\n", cmd, almt, len, rspn);
	
	#if 1
	do	{
		rspn = SSP0byte(0xff);
		//uprintf("%02x ", rspn);
	} while (rspn!= 0xFE);
	//uprintf("-------------\r\n");
	#endif
	
	//hsl[0] = rspn;
	data[0] = rspn;
	rspn = SSP0byte(0xff);
	for(a=1; a<512; a++)	{
		rspn = SSP0byte(0xff);
		//hsl[a] = rspn;
		data[a] = rspn;
	}
	#if 0
	int i=0;
	while (i<512)	  {
		data[i] = hsl[i];
		i++;
	}
	#endif
	#if 0
	for (a=0; a<32; a++)	{
		for (b=0; b<16; b++)	{
			uprintf("%02x ", data[a*16+b]);
			if (b==7) uprintf("  ");
		}
		//uprintf("%02x ", hsl[a]);
		uprintf("\r\n");
	}
	#endif
	
	flush_ssp_spi(); 

	
	//printf("------------> seleasai baca sdc %02x %02x == %02x %02x\r\n", hsl[510], hsl[511], data[510], data[511]);
	
	#if 0
	if (send_cmd(cmd, sector, 1, &resp)		/* Start to read */
		&& !(resp & 0xC0580000)) {
		rp = 0;
		do {
			while ((rp == XferWp) && !(XferStat & 0xC)) {	/* Wait for block arrival */
				/* This loop will take a time. Replace it with sync process for multitask envilonment. */
			}
			if (XferStat & 0xC) break;			/* Abort if any error has occured */
			Copy_al2un(buff, DmaBuff[rp], 512);	/* Pop an block */
			XferRp = rp = (rp + 1) % N_BUF;		/* Next DMA buffer */
			if (XferStat & 0xC) break;			/* Abort if overrun has occured */
			buff += 512; 						/* Next user buffer address */
		} while (--count);
	}
	stop_transfer();						/* Close data path */
	if (count || cmd == CMD18)				/* Terminate to read if needed */
		send_cmd(CMD12, 0, 1, &resp);

	return count ? RES_ERROR : RES_OK;
	
	#endif
	
	#if 0
	rspn = Microsd_SendCmd( SET_BLOCKLEN, len );
	if (rspn != R1_READY_STATE)
		return rspn;
	rspn = Microsd_SendCmd(READ_SINGLE_BLOCK, almt);
	if(rspn != R1_READY_STATE)
		return rspn;
	rspn = respon_SDcmd(data, len);
	if (rspn != START_TOKEN)	{		// START_TOKEN: 0xFE
		qsprintf("__START TOKEN salah !!__\r\n");
		return rspn;
	}
	
	rspn = SSP0byte(0xff);
	crc = rspn;
	rspn = SSP0byte(0xff);
	crc = (crc << 8) | rspn;
	
	if( crc != Microsd_crc16(data, len) ) {
		qsprintf("__ReadBlock(): CRC error, data dumped__\r\n");
		return 0x00;
	}
	
	return 0xff;
	#endif
	return 0;
}


uchr tulis_block_sdc(unsigned int almt, uchr *data, int len)	{
	uchr rspn;
	unsigned int i;
	unsigned short crc;
	
	rspn = Microsd_SendCmd( SET_BLOCKLEN, len );
	if (rspn != R1_READY_STATE)
		return rspn;
	rspn = Microsd_SendCmd( WRITE_BLOCK, almt );
	if(rspn != R1_READY_STATE) {
		qsprintf("__START TOKEN salah__, respon:%02x !!\r\n", rspn);
		return rspn;
	}
	SSP0byte(0xff);
	SSP0byte(0xff);
	
	SSP0byte(0xfe);
	for(i = 0; i < len; i++)	{
		SSP0byte(&data[i]);
	}
	
	crc = Microsd_crc16(data, len);
	rspn = (crc >> 8) & 0xff;
	SSP0byte(rspn);
	rspn = (crc & 0xff);
	rspn = SSP0byte(rspn);
	
	if ( (rspn & 0x0f) == 0x05 )	{
		qsprintf(" BerHASIL simpan data\r\n");
	} else if ( (rspn & 0x0f) == 0x0B )	{
		qsprintf(" __CRC Error__\r\n");
	} else if ( (rspn & 0x0f) == 0x05 )	{
		qsprintf(" __Simpan Error__\r\n");
	} else {
		qsprintf(" respon aneh : %02x.\r\n", rspn);
	}
	
	do	{
		rspn = SSP0byte(0xff);
	} while (rspn == 0x00);
	
	
	return 0x00;
	
}

unsigned char cek_versi2_sdc(void)		{
	int i;
	unsigned char strRsp[4];
	unsigned char versi=0, cc=0, rsp=0;
	
	unsigned char strHrp[] = { 0x00, 0x00, 0x01, 0xAA };
	Microsd_SendCmd( CMD8, 0x000001AA );
	rsp = respon_SDcmd( strRsp, R7_RESPONSE_SIZE );
	uprintf("\r\nhasil respon : %02x %02x %02x %02x\r\n", strRsp[0], strRsp[1], strRsp[2], strRsp[3]);
	
	if (rsp == R7_RESPONSE_SIZE)	{
		for (i=0; i<R7_RESPONSE_SIZE; i++)		{
			if (strRsp[i] == strHrp[i])
				cc++;
		}
		
		//uprintf("cc: %d\r\n", cc);
		if (cc==R7_RESPONSE_SIZE)	{
			uprintf("support SDC v2. ");
			versi = 2;
		} else {
			uprintf("tidak support SDC v2, cc: %d ", cc);
			versi = 0;
		}
	}
	
	return versi;
}


unsigned char SD_WriteCmd(unsigned char* cmd)	{
	unsigned int i;
	unsigned char response;
	
	//cmd[0] |= (1<<6);
	//cmd[0] &= ~(1<<7);
	cmd[0]  = (cmd[0] & 0x3F) | 0x40;
	cmd[5] |= (1<<0);
	
	

	#ifdef DEBUG_SDC
	vTaskDelay(1);
	//uprintf(".");
	//uprintf("CMD: %02x %02x %02x %02x %02x %02x\r\n", cmd[0],cmd[1],cmd[2],cmd[3],cmd[4],cmd[5]);
	#endif
		
	for(i = 0; i < 6; ++i)	{
		SSP0Send(*cmd, 1);
		//uprintf("%02x ", *cmd);
		response = SSP0Receive();
		//uprintf("%02x ", response);
		cmd++;
	}
	
	// Wait for the response
	i = 0;
	do	{
		//response = SPIRead();
		SSP0Send(0xff, 1);
		response = SSP0Receive();
		//uprintf("%02x ", response);
		if(i > 50)		{
			break;
		}
		i++;
	} while(response == 0xFF);
	
	#ifdef DEBUG_SDC
	//uprintf("respon[%d]: %02x\r\n", i, response);
	#endif
	// Following any command, the SD Card needs 8 clocks to finish up its work.
	// (from SanDisk SD Card Product Manual v1.9 section 5.1.8)
	//SPIWrite(0xFF); 
	SSP0Send(0xff, 1);

	//SD_CS = savedSD_CS;
	return (response);
}


void flush_ssp_spi()	{
	int i;
	unsigned char rsp;
	//uprintf("\r\nFlush !!! : ");
	for(i=0; i<8; i++)	{
		rsp = SSP0byte(0xff);
		//uprintf("%02x ", rsp);
	}
	//uprintf("\r\n");
}

void sdc_infox()		{
	int i;
	unsigned char status;
	unsigned char cid[20];			// Card Identification Register
    unsigned char csd[20];			// Card Specific Data Register
    unsigned char scr[8];			// SD card configuration register

    
	status = Microsd_SendCmd_nocrc( CSD, 0x00000000 );
	respon_SDcmd( csd, 20 );
	flush_ssp_spi();
	
	status = Microsd_SendCmd_nocrc( CID, 0x00000000 );
	respon_SDcmd( cid, 20 );
	flush_ssp_spi();
    
    status = Microsd_SendCmd( SCR, 0x00000000 );
	respon_SDcmd( scr, 4 );
	uprintf("\r\nCMD%d : SCR : %02x : ", SCR, status);
	for(i=0; i<4; i++)	{
		uprintf("%02x ", cid[i]);
	}
	flush_ssp_spi();

	
	uprintf("\r\nCMD%d : CSD : %02x : ", CSD, status);
	for(i=0; i<20; i++)		uprintf("%02x ", csd[i]);
	for(i=0; i<20; i++)		{
		uprintf("%02x ", csd[i]);
		if (csd[i]==START_TOKEN)	{
			++i;
			break;
		}
	}
	uprintf("\r\nCID : TOKEN 0xFE i=%d\r\n", i);
	unsigned int Blocks;
    if ( (csd[i+0] & 0xc0) == 0) {		// ff fe 00
		//Type 1 csd
		unsigned int C_Size      = ((csd[i+6] & 0x03) << 10) | (csd[i+7] << 2) | ((csd[i+8] >>6) & 0x03);
		unsigned int C_Size_Mult = ((csd[i+9] & 0x03) << 1) | ((csd[i+10] >> 7) & 0x01);
		unsigned int Read_Bl_Len = csd[i+5] & 0x0f;
		Blocks = ((C_Size + 1) * (1 << (C_Size_Mult + 2)) * (1 << Read_Bl_Len)) / 512;
		uprintf("CSD tipe 1, block: %d, BL: %d\r\n", Blocks, Read_Bl_Len);
    } else {
		//Type 2 csd
		Blocks = (((csd[i+7] & 0x3F) << 16) | (csd[i+8] << 8) | csd[i+9]) * 1024;
		unsigned int C_Size = (((csd[i+7] & 0x3F) << 16) | (csd[i+8] << 8) | csd[i+9]);
		uprintf("CSD tipe 2, block: %d, Csize: %d, %06x, T: %d\r\n", Blocks, C_Size, C_Size, C_Size*512);
    }
	cInfo.speed = csd[i+3];
	cInfo.sector = ((csd[i+10] & 0xFC) << 2) | ((csd[i+11] & 0x1) >> 7) ;		// 
	uprintf("speed: 0x%02x, sector: %d\r\n", cInfo.speed, cInfo.sector);

	//c->state |= (CARD_STATE_DET & CARD_STATE_INI); 		// State - detected/initialized
	//c->type |= CARD_TYPE_SD;					// SD card
	//if (ocr[0] & 0x40) c->type |= CARD_TYPE_HC;			// std/high capacity
	//c->version = scr[0] & 0x0f;					// Version
	//c->volt = (ocr[1] << 16) + (ocr[2] << 8) + ocr[3];		// Voltage range from OCR

	#if 1
	uprintf("\r\nCID : ");
	for(i=0; i<20; i++)		uprintf("%02x ", cid[i]);
	uprintf("\r\nCMD%d : CID : %02x : ", CID, status);
	#endif
	for(i=0; i<20; i++)		{
		//uprintf("%02x ", cid[i]);
		if (cid[i]==START_TOKEN)	{	
			++i;
			break;
		}
	}
	uprintf("\r\nCID : TOKEN 0xFE i=%d\r\n", i);
	cInfo.blocks = Blocks;						// Size in 512 byte blocks
	cInfo.manid = cid[i+0];		 						// Manufacturer ID
	memcpy(cInfo.name, &cid[i+3], 5);	 cInfo.name[5] = '\0';	// Product Name
	//memcpy(c.appid, &cid[i+1], 2);						// OEM/Application ID
	cInfo.oem = (cid[i+1] << 8) | cid[i+2];
	cInfo.rev = cid[i+8];									// Product Revision
	cInfo.year = 2000 + ((cid[i+13] & 0x0f) << 4) | (cid[i+14] >> 4);	// Product Date - year
	cInfo.month = (cid[i+14] & 0x0f);					// Product Date - month
	cInfo.serial = (cid[i+9] << 24) | (cid[i+10] << 16) | (cid[i+11] << 8) | cid[i+12]; // Serial Number
	uprintf("MID: %d, Nama: %s, Rev%d, SN: %ld, ManDate: %02d-%d, oem: 0x%04X-%02x%02x\r\n", \
		cInfo.manid, cInfo.name, cInfo.rev, cInfo.serial, cInfo.month, cInfo.year, cInfo.oem, cInfo.appid[0], cInfo.appid[1]);


}

unsigned char cek_status_sdc(void)	{
	if (cek_ins_sdc())	{		// cek kartu ada ?
		unsigned char CMD0_GO_IDLE_STATE[] = { 0x00,0x00,0x00,0x00,0x00,0x95 };
		if  (SD_WriteCmd(CMD0_GO_IDLE_STATE) == 0x01)
			return 0;
		else return 1;
	}
	return 1;
}

unsigned char init_sdc(void)		{
	#ifdef DEBUG_SDC
	uprintf("--> %s\r\n", __FUNCTION__);
	#endif
	
	unsigned int i = 0;
	unsigned char status, v2=0;
	
	setup_spi_ssp0();
	init_ssp0(SCK_LAMA);

	for(status = 0; status < 10; ++status)	{
		for(i = 0; i; i++);
		for(i = 0; i; i++);
		for(i = 0; i; i++);
	}
	
	for(i = 0; i < 16; ++i)
		SSP0Send(0xFF, 1);			// write dummy data to pump clock signal line
	
	// Wait for the SD Card to go into IDLE state
	i = 0;	status = 0xff;
	do	{
		unsigned char CMD0_GO_IDLE_STATE[] = { 0x00,0x00,0x00,0x00,0x00,0x95 };
		status = SD_WriteCmd(CMD0_GO_IDLE_STATE);
		//status = Microsd_SendCmd( CMD0, 0x00000000);
		// fail and return
		if (i++ > 25)	{
		//if (i++ > 10000)	{
			return 1;
		}
	} while( status != 0x01 );
	//#ifdef DEBUG_SDC
	#if 1
	uprintf("---> CMD0_GO_IDLE_STATE status[%d] : %d\r\n", i, status);
	#endif
	v2 = cek_versi2_sdc();
	//uprintf("v2: %02x\r\n", v2);
	flush_ssp_spi();
	
	i=0;
	do	{
		unsigned char CMD55_APP_CMD[] = {55,0x00,0x00,0x00,0x00,0xFF};
		status = SD_WriteCmd(CMD55_APP_CMD); // Do not check response here
		#ifdef DEBUG_SDC
		uprintf("---> CMD55_APP_CMD status[%d] : %d\r\n", i, status);
		#endif
		if (status==0x01)	{
			unsigned char cmdSDcmd41[] = { 41,0x40,0x00,0x00,0x00,0xe5 };		// kingston 4GB
			status = SD_WriteCmd(cmdSDcmd41);
			#ifdef DEBUG_SDC
			uprintf("---> CMD41_SD_SEND_OP_COND status[%d] : %02x\r\n", i, status);
			#endif
		}
		// fail and return
		if (i++ > 20)	{
			return 2;
		}
	} while( status != 0x00 );
	flush_ssp_spi();

	//vTaskDelay(50);
	init_ssp0(SCK_KILAT);
	st_hw.sdc = 1;

	vTaskDelay(50);


	#if 1
	unsigned char strRsp[4];
	status = Microsd_SendCmd( CMD58, 0x40000000 );
	respon_SDcmd( strRsp, R3_RESPONSE_SIZE );
	uprintf("---> 8 st: %02x, respon : %02x %02x %02x %02x\r\n", status, strRsp[0], strRsp[1], strRsp[2], strRsp[3]);
	if (status!=0)
		return 3;
	memcpy(cInfo.ocr, &strRsp[0], 4);
	//c.CardType = (resp[0] & 0x40) ? CT_SD2|CT_BLOCK : CT_SD2;
	
	#endif
	flush_ssp_spi();
	
	sdc_info();
	flush_ssp_spi();


	return 0;
}


unsigned char Microsd_crc7(unsigned char *MessBuff)	{
	int i;
	unsigned char	Upper8;
	unsigned int Lower32;

	Upper8 = MessBuff[0];
	Lower32 = ( (unsigned int) MessBuff[1] << 24 ) |
			  ( (unsigned int) MessBuff[2] << 16 ) |
			  ( (unsigned int) MessBuff[3] << 8  ) |
			  ( (unsigned int) MessBuff[4] );

	for(i = 0; i < 40; i++)		{
		if(Upper8 & 0x80) // if the MSB of the bit-frame is 1...
			Upper8 ^= G_POLY7; // xor with the generator polynomial

		/* Logical left shift of the bit-frame by 1 bit */
		Upper8 <<= 1;

		if(Lower32 & 0x80000000) // if the MSB of MessPtr->Lower32 is 1...
			Upper8 |= 0x01; // set the LSB of Upper8 to 1
			Lower32 <<= 1;
	}

	return Upper8;
}

unsigned short Microsd_crc16(unsigned char *MessBuff, unsigned int len)	{
	unsigned short crc = 0;
	int byten, bitn, xorFlag;

	if(len <= 1) {
		qsprintf("crc16() does not handle 1-byte messages.\r\n"); return 0; }

	for(byten = 0; byten < len+2; byten++) // Append 2-bytes to end of message.
		for(bitn = 0; bitn < 8; bitn++)		{
			xorFlag = crc & 0x8000;
			crc <<= 1;

			if((byten < len) && (MessBuff[byten] & (0x80 >> bitn)))
				crc |= 0x0001;

			if(xorFlag)
				crc ^= G_POLY16;

		}// end for(bitn)
	return crc;
}

unsigned char Microsd_SendCmd(unsigned char cmd_idx, unsigned int arg)	{
	int i = 0;
	unsigned char MessBuff[6], RecvBuff[6];

	//Microsd_FlushRxFifo(SDPtr);

	// Append start (zero) and transmission (one) bits to front,
	// insert argument to MessBuff[1:4], append CRC and stop bit (one) to end.
	MessBuff[0] = (cmd_idx & 0x3F) | 0x40;
	MessBuff[1] = (unsigned char) ((arg >> 24) & 0x000000FF);
	MessBuff[2] = (unsigned char) ((arg >> 16) & 0x000000FF);
	MessBuff[3] = (unsigned char) ((arg >>  8) & 0x000000FF);
	MessBuff[4] = (unsigned char) ( arg & 0x000000FF);
	MessBuff[5] = ( Microsd_crc7(MessBuff) | 0x01 );

	#if 0
	uprintf("----> %s : ", __FUNCTION__);
	for(i=0; i<6; i++)	{
		uprintf("%02x ", MessBuff[i]);
	}
	uprintf("\r\n");
	#endif
	//Microsd_SpiTransfer(SDPtr, MessBuff, RecvBuff, 6);
	return SD_WriteCmd(MessBuff);

	//return Microsd_GetResponse(SDPtr);
}

unsigned char Microsd_SendCmd_nocrc(unsigned char cmd_idx, unsigned int arg)	{
	int i = 0;
	unsigned char MessBuff[6], RecvBuff[6];

	//Microsd_FlushRxFifo(SDPtr);

	// Append start (zero) and transmission (one) bits to front,
	// insert argument to MessBuff[1:4], append CRC and stop bit (one) to end.
	MessBuff[0] = (cmd_idx & 0x3F) | 0x40;
	MessBuff[1] = (unsigned char) ((arg >> 24) & 0x000000FF);
	MessBuff[2] = (unsigned char) ((arg >> 16) & 0x000000FF);
	MessBuff[3] = (unsigned char) ((arg >>  8) & 0x000000FF);
	MessBuff[4] = (unsigned char) ( arg & 0x000000FF);
	MessBuff[5] = ( 0xff | 0x01 );

	#if 1
	for(i=0; i<6; i++)	{
		uprintf("%02x ", MessBuff[i]);
	}
	uprintf("\r\n");
	#endif
	//Microsd_SpiTransfer(SDPtr, MessBuff, RecvBuff, 6);
	return SD_WriteCmd(MessBuff);

	//return Microsd_GetResponse(SDPtr);
}

#endif
