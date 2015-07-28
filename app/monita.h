
#ifndef __APP_MONITA__
#define __APP_MONITA__

#include <time.h>

#define JML_SECTOR_LPC 		28
#define IAP_ADDRESS 		0x7FFFFFF1
typedef void (*IAP)(unsigned int [],unsigned int[]);



#ifdef PAKAI_SDCARD
#include "ff/ff9b/src/ff.h"
//FATFS xFatFs[2];
//FATFS Fatfs[1];
#endif

typedef struct	{
  unsigned int ReturnCode;
  unsigned int Result[4];
} IAP_return_t;

typedef enum IAP_STATUS_t {
   CMD_SUCCESS = 0,         // Command was executed successfully.
   INVALID_COMMAND = 1,     // Invalid command.
   SRC_ADDR_ERROR = 2,      // Source address is not on a word boundary.
   DST_ADDR_ERROR = 3,      // Destination address is not on a correct boundary.

   SRC_ADDR_NOT_MAPPED = 4, // Source address is not mapped in the memory map.
                            // Count value is taken in to consideration where
                            // applicable.

   DST_ADDR_NOT_MAPPED = 5, // Destination address is not mapped in the memory
                            // map. Count value is taken in to consideration
                            // where applicable.

   COUNT_ERROR = 6,         // Byte count is not multiple of 4 or is not a
                            // permitted value.

   INVALID_SECTOR = 7,      // Sector number is invalid.
   SECTOR_NOT_BLANK = 8,    // Sector is not blank.

   SECTOR_NOT_PREPARED_FOR_WRITE_OPERATION = 9, // Command to prepare sector for
                                                // write operation was not
                                                // executed.

   COMPARE_ERROR = 10,      // Source and destination data is not same.
   BUSY = 11,               // Flash programming hardware interface is busy.
} IAP_STATUS_t;

//IAP_return_t iap_return;
short astm_aktif;

#define  RPM_MAX		3000
#define  PROP_MAX		250
#define  nFLOW_MAX		10000000

#define  YA				1
#define  TIDAK			0

#define  sRPM			1
#define	 sONOFF			2
#define	 sPUSHBUTTON	3
#define	 sFLOW1			4
#define	 sFLOW2			5
#define  sFLOWx			6
#define	 sRPM_RH		7
#define	 sRUNNING_HOURS	8
#define	 sONOFF_RH		9
#define	 sPROP			10
#define	 nFLOW1			100
#define	 ssFLOW2		201
#define	 nFLOW2			202
#define	 fFLOW			203
#define	 DAYA			6
#define	 sADC_RH		230
#define	 sADC_ORI		240
#define  sADC_7708		250
#define  fENERGI		251

#define  FOLDER_SENDED		"\\terkirim"
#define	 LIHAT				0
#define  LIHAT_ISI_SATU		1
#define  MAX_SEND_FILE_MB	3600
#define  MAX_RX_MB			1*1024
#define  MAX_DEBUG_TX		2*1024

#define  RTC_MEM_START		100

#define KIRIM_FILE_MULAI_WAKTU	"H-10"

//#define ATA		0
//#define MMC		1
//#define USB		2
#define ROM		1
#define SDC		0

#define TIDAK_VALID			0xFFFF
#define PER_SUMBER			10
//#define PER_ASTM			85
//#define PER_ASTM			91 //jumlah koefisien
#define PER_ASTM			10 
#define BLOK_ASTM			21
#define JML_SUMBER			 6
#define SUMBER_PER_SEKTOR	32

//#define ANGKA_PENTING	1

#ifdef PAKAI_RELAY
	#define JML_RELAY		8
	#define JML_TITIK_DATA	(JML_SUMBER * PER_SUMBER)+JML_RELAY
#else
	#define JML_TITIK_DATA	(JML_SUMBER * PER_SUMBER)
#endif

volatile float data_f [ JML_TITIK_DATA ];
volatile float astm_f [ 12 ];
//volatile int nFL;

char strmb[MAX_RX_MB];//		__attribute__ ((section (".usbram1")));
char outmb[MAX_RX_MB];//		__attribute__ ((section (".usbram1")));


// 32 KB
#define ALMT_SEKTOR_8	0x08000	
#define ALMT_SEKTOR_9	0x10000
#define ALMT_SEKTOR_10	0x18000
#define ALMT_SEKTOR_11	0x20000
#define ALMT_SEKTOR_12	0x28000
#define ALMT_SEKTOR_13	0x30000
#define ALMT_SEKTOR_14	0x38000
#define ALMT_SEKTOR_15	0x40000
#define ALMT_SEKTOR_16	0x48000
#define ALMT_SEKTOR_17	0x50000
#define ALMT_SEKTOR_18	0x58000
#define ALMT_SEKTOR_19	0x60000
#define ALMT_SEKTOR_20	0x68000
#define ALMT_SEKTOR_21	0x70000

// 4 KB
#define ALMT_SEKTOR_22	0x78000
#define ALMT_SEKTOR_23	0x79000
#define ALMT_SEKTOR_24	0x7A000
#define ALMT_SEKTOR_25	0x7B000
#define ALMT_SEKTOR_26	0x7C000
#define ALMT_SEKTOR_27	0x7D000

#define SEKTOR_ENV		19
#define ALMT_ENV		(ALMT_SEKTOR_19)
#define ALMT_SUMBER		(ALMT_SEKTOR_19+1024*1)
#define ALMT_CRON		(ALMT_SEKTOR_19+1024*2)
#define ALMT_FILE		(ALMT_SEKTOR_19+1024*3)

#if 1
#define SEKTOR_ASTM		18
#define ALMT_VALUE_ASTM	(ALMT_SEKTOR_18)
#endif

enum t_struct{ 
	DATA,
	ENV,
	SUMBER,
	CRON,
	BERKAS,
	ASTM
	}; 
enum t_struct st_struct;

#define SEKTOR_DATA		20
#define ALMT_DATA		ALMT_SEKTOR_20

#define SEKTOR_TEMP		21
#define ALMT_SKTR_TEMP	ALMT_SEKTOR_21
#define JML_KOPI_TEMP	1024
#define JML_KOPI_ASTM	512
#define ALMT_ENV_TMP		(ALMT_SEKTOR_21)
#define ALMT_DATA_TMP		(ALMT_SEKTOR_21)
#define ALMT_SUMBER_TMP		(ALMT_SEKTOR_21+1024*1)
#define ALMT_CRON_TMP		(ALMT_SEKTOR_21+1024*2)
#define ALMT_FILE_TMP		(ALMT_SEKTOR_21+1024*3)


#define JUM_DIGITAL	12
#define JML_KANAL	10
#define TOT_ANDIG	22

#define uchr		unsigned char
//#define uint		unsigned int



typedef struct {
	//unsigned int new_period;
	unsigned int last_period;
	unsigned int beda;			// kali 10
	unsigned int hit;
	unsigned int hit_lama;		// untuk mengetahui mati atau enggak
	unsigned char onoff;
	unsigned int hit2;
	unsigned int hit_lama2;		// untuk mengetahui mati atau enggak
	time_t rh_on;				// untuk mengetahui waktu aktif
	time_t rh_off;				// untuk mengetahui waktu mati
	unsigned int rh_flag;		// untuk mengetahui FSM rh
	time_t rh;					// untuk mengetahui nilai running hours, setelah running, mulai 0 ketika sebelumnya mati
	time_t rh_x;				// untuk mengetahui nilai running hours, total sebelum running lagi
} ts_konter;

struct t2_konter{
	unsigned int global_hit;
	unsigned int ovflow;		// overflow count untuk Timer 1
	ts_konter t_konter[JUM_DIGITAL];
};

unsigned int giliran;
struct t2_konter konter;
float k_t0[3];
float k_t1[3];
float d_t0[3];
float d_t1[3];

//unsigned char status_konter[JML_KANAL];

struct t_st_hw  {
	unsigned char init;
	long int idle;
	long int idle_c;
	unsigned char adc;
	long int adc_pd;
	long int adc_c;
	unsigned char rtc;
	unsigned char sdc;
	int mm;
	time_t wkt_awal;
	time_t wkt_now;
	int uudetik;
	int uuwaktu;
};

volatile struct t_st_hw st_hw;

struct t_kalib {
	float m;
	float C;
	char status;		// [status lihat define di atas !!!] //
	char adc;
};

#ifdef PAKAI_FILE_SIMPAN
struct t_file	{
	int jml;
	int simpan;
	int urut[JML_TITIK_DATA];
};
#endif

struct t_adc {
	unsigned char cur_kanal;
	unsigned int count;
	unsigned char ambil;
	unsigned short data[JML_KANAL];
	time_t rh_on[JML_KANAL];				// untuk mengetahui waktu aktif
	time_t rh_off[JML_KANAL];				// untuk mengetahui waktu mati
	unsigned int rh_flag[JML_KANAL];		// untuk mengetahui FSM rh
	time_t rh[JML_KANAL];					// untuk mengetahui nilai running hours, setelah running, mulai 0 ketika sebelumnya mati
	time_t rh_x[JML_KANAL];				// untuk mengetahui nilai running hours, total sebelum running lagi
	//float flt_data[JML_KANAL];
};
struct t_adc adc;

struct t_data {
	unsigned int id;
	char satuan[6];
	int  rangeL;
	int  batasLL;
	int  batasL;
	int  batasH;
	int  batasHH;
	int  rangeH;
	char nama[24];
	char status;			// 
	char formula[16];
};
//struct t_data st_data[JML_TITIK_DATA];

struct t_astm
	{
	//float press;
	//float temp;
	float koef;
	};

#ifdef PAKAI_CRON
struct t_cron {
	char 	mnt[20];
	char 	jam[20];
	char	tgl[20];
	char	bln[20];
	char	cmd[20];
	char 	alamat;
	char	status;			// status perintahnya : suruh dihidup ato mati
	char 	set;			// status cron : aktif atau mati
};
#endif 

struct t_sumber {
	char nama[32];
	char alamat;		/* untuk alamat modbus Power meter atau stack board (jika ada) */
	unsigned char IP0;			// klo sumber berupa modul monita 
	unsigned char IP1;
	unsigned char IP2;
	unsigned char IP3;
	//char modul;			// khusus modbus, jenis modul 0: PM, 1: KTA, 2:????, dst
	char stack;			// jika modul berisi BANYAK_SUMBER : adc, pm, dll
	char status;		// tidak aktif, timeout, dll
	char tipe;			// 0:PM_710, 1:PM_810, 2:KTA, 3:MICOM
	char ket[32];
};
//struct t_sumber st_sumber[JML_SUMBER];

struct t_env {
	char nama_board[32];
	unsigned char IP0;
	unsigned char IP1;
	unsigned char IP2;
	unsigned char IP3;
	unsigned char GW0;
	unsigned char GW1;
	unsigned char GW2;
	unsigned char GW3;
	struct t_kalib kalib[TOT_ANDIG];
	int magic1;
	int magic2;
	int mmc_serial;
	char SN[20];
	char berkas[32];
	char statusWebClient;
	char banyak_sumber;
	char burst;
	unsigned char wIP0;
	unsigned char wIP1;
	unsigned char wIP2;
	unsigned char wIP3;
	char statusWebClientI;
	unsigned char k1;
	unsigned char k2;
	unsigned char uRTC;
	char	statusSerClient;
	int		intKirim;
	int		intReset;
	float	intTole;
	int		netTot;
	char passwd[15];
	char madein[18];
	char nohp[18];
	char statusCron;
	unsigned char almtSlave;
	char statusSlave;
	int		prioDebug;
	int		prioDebug2;
	int		jmlfile;
	int 	fuel_den;
	int 	jumFlow;
	int 	nFL;
};
//struct t_env st_env;


#ifdef PAKAI_RTC
	unsigned char flagRTCc;
	unsigned char flagRTCs;
	unsigned char flagRTCm;
	unsigned char flagRTCh;
	unsigned char flagRTCd;
	
typedef struct __attribute__ ((packed)) {
	union  {
		struct	{
			unsigned int counter   : 14;
			unsigned int rsvd15_31 : 18;
		};
		unsigned int i;
	};
} rtcCTC_t;

typedef struct __attribute__ ((packed))	{
	union	{
		struct	{
			unsigned int seconds   : 6;
			unsigned int rsvd7_6   : 2;
			unsigned int minutes   : 6;
			unsigned int rsvd14_15 : 2;
			unsigned int hours     : 5;
			unsigned int rsvd21_23 : 3;
			unsigned int dow       : 3;
			unsigned int rsvd27_31 : 5;
		};
		unsigned int i;
	};
} rtcCTIME0_t;

typedef struct __attribute__ ((packed))	{
	union	{
		struct	{
			unsigned int dom       : 5;
			unsigned int rsvd5_7   : 3;
			unsigned int month     : 4;
			unsigned int rsvd12_15 : 4;
			unsigned int year      : 12;
			unsigned int rsvd28_31 : 4;
		};
		unsigned int i;
	};
} rtcCTIME1_t;

typedef struct __attribute__ ((packed))	{
	union	{
		struct 	{
			unsigned int doy       : 12;
			unsigned int rsvd12_31 : 20;
		};
		unsigned int i;
	};
} rtcCTIME2_t;

struct rtc_time {
  int	tm_sec;
  int	tm_min;
  int	tm_hour;
  int	tm_mday;	// 1 - 31 /
  int	tm_mon;		// 0 - 11 //
  int	tm_year;
  int	tm_wday;
  int	tm_yday;
  int	tm_isdst;
};

#endif

#endif
