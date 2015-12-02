/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2013        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control module to the FatFs module with a defined API.        */
/*-----------------------------------------------------------------------*/

#include "FreeRTOS.h"

//#include <time.h>

/* Definitions of physical drive number for each media */

#ifdef PAKAI_SDCARD

#include "diskio.h"		/* FatFs lower layer API */
//#include "usbdisk.h"	/* Example: USB drive control */
//#include "atadrive.h"	/* Example: ATA drive control */
//#include "sdcard.h"		/* Example: MMC/SDC contorl */
#include "ff/sdc.h"
#include "monita.h"

/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber (0..) */
)
{
	DSTATUS stat;
	int result;

	switch (pdrv) {
	#ifdef PAKAI_ATA
	case ATA :
		result = ATA_disk_initialize();

		// translate the reslut code here

		return stat;
	#endif
	#ifdef PAKAI_MMC
	case MMC :
		result = MMC_disk_initialize();

		// translate the reslut code here

		return stat;
	#endif
	#ifdef PAKAI_USB_STORAGE
	case USB :
		result = USB_disk_initialize();

		// translate the reslut code here

		return stat;
	#endif
	case SDC :
		// translate the reslut code here
		//return init_sdc();
		return sdc_init();
	
	case ROM :
		return stat;
	}
	return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Get Disk Status                                                       */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber (0..) */
)
{
	DSTATUS stat;
	int result;

	switch (pdrv) {
	#ifdef PAKAI_ATA
	case ATA :
		result = ATA_disk_status();

		// translate the reslut code here

		return stat;
	#endif
	#ifdef PAKAI_MMC
	case MMC :
		result = MMC_disk_status();

		// translate the reslut code here

		return stat;
	#endif
	#ifdef PAKAI_USB_STORAGE
	case USB :
		result = USB_disk_status();

		// translate the reslut code here

		return stat;
	#endif
	case SDC :
		// translate the reslut code here
		return cek_status_sdc();
	
	}
	return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address (LBA) */
	BYTE count		/* Number of sectors to read (1..128) */
)
{
	DRESULT res;
	int result;

	switch (pdrv) {
	#ifdef PAKAI_SDCARD
	case SDC :
		return sdc_read(buff, sector, count);		// disk_read(fs->drv, fs->win, sect, 1)
	#endif
	
	#ifdef PAKAI_ATA
	case ATA :
		// translate the arguments here

		result = ATA_disk_read(buff, sector, count);

		// translate the reslut code here

		return res;
	#endif
	#ifdef PAKAI_MMC
	case MMC :
		// translate the arguments here

		result = MMC_disk_read(buff, sector, count);

		// translate the reslut code here

		return res;
	#endif
	#ifdef PAKAI_USB_STORAGE
	case USB :
		// translate the arguments here

		result = USB_disk_read(buff, sector, count);

		// translate the reslut code here

		return res;
	#endif
	}
	return RES_PARERR;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if _USE_WRITE
DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber (0..) */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Sector address (LBA) */
	BYTE count			/* Number of sectors to write (1..128) */
)
{
	DRESULT res;
	int result;

	switch (pdrv) {
	#ifdef PAKAI_ATA
	case ATA :
		// translate the arguments here

		result = ATA_disk_write(buff, sector, count);

		// translate the reslut code here

		return res;
	#endif
	#ifdef PAKAI_MMC
	case MMC :
		// translate the arguments here

		result = MMC_disk_write(buff, sector, count);

		// translate the reslut code here

		return res;
	#endif
	#ifdef PAKAI_USB_STORAGE
	case USB :
		// translate the arguments here

		result = USB_disk_write(buff, sector, count);

		// translate the reslut code here

		return res;
	#endif
	}
	return RES_PARERR;
}
#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

#if _USE_IOCTL
DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT res;
	int result;

	switch (pdrv) {
	#ifdef PAKAI_ATA
	case ATA :
		// pre-process here

		result = ATA_disk_ioctl(cmd, buff);

		// post-process here

		return res;
	#endif
	#ifdef PAKAI_MMC
	case MMC :
		// pre-process here

		result = MMC_disk_ioctl(cmd, buff);

		// post-process here

		return res;
	#endif
	#ifdef PAKAI_USB_STORAGE
	case USB :
		// pre-process here

		result = USB_disk_ioctl(cmd, buff);

		// post-process here

		return res;
	#endif
	#ifdef PAKAI_SDCARD
	case SDC :
		// pre-process here

		result = SDC_ioctl(cmd, buff);

		// post-process here

		return res;
	#endif
	}
	return RES_PARERR;
}

#endif


//unsigned int get_fattime (void)	{
DWORD get_fattime (void)	{
	unsigned int tw;
	struct rtc_time waktu;
	
	rtcCTIME0_t ctime0;
	rtcCTIME1_t ctime1;
	rtcCTIME2_t ctime2;
	ctime0.i = RTC_CTIME0; 
	ctime1.i = RTC_CTIME1; 
	ctime2.i = RTC_CTIME2;
	
	waktu.tm_sec  = ((int) (ctime0.seconds >> 1)) & 0x1F;
	waktu.tm_min  = (ctime0.minutes << 5) & 0x7E0;
	waktu.tm_hour = (ctime0.hours   << 11) & 0xF800;
	waktu.tm_mday = (ctime1.dom     << 16) & 0x1F0000;
	waktu.tm_mon  = ((ctime1.month+1) << 21) & 0x1E00000;
	waktu.tm_year = ((ctime1.year-1980) << 25) & 0xFE000000;
	
	tw = waktu.tm_year | waktu.tm_mon | waktu.tm_mday | waktu.tm_hour | waktu.tm_min | waktu.tm_sec;
	return tw;
}

#endif
