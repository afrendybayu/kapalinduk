
#ifndef __M_SDCARD__
#define __M_SDCARD__

#define R1_IN_IDLE_STATE    (1<<0)   // The card is in idle state and running initializing process.
#define R1_ERASE_RESET      (1<<1)   // An erase sequence was cleared before executing because of an out of erase sequence command was received.
#define R1_ILLEGAL_COMMAND  (1<<2)   // An illegal command code was detected
#define R1_COM_CRC_ERROR    (1<<3)   // The CRC check of the last command failed.
#define R1_ERASE_SEQ_ERROR  (1<<4)  // An error in the sequence of erase commands occured.
#define R1_ADDRESS_ERROR    (1<<5)  // A misaligned address, which did not match the block length was used in the command.
#define R1_PARAMETER        (1<<6)  // The command's argument (e.g. address, block length) was out of the allowed range for this card

#define SD_STATUS_READY_MASK		0x01
#define SD_STATUS_LEGACY_MASK		0x02
#define SD_STATUS_HIGHCAP_MASK		0x04
#define SD_STATUS_FAT_READY_MASK	0x10
#define SD_STATUS_UNUSABLE_MASK		0x80

#define GO_IDLE_STATE		0
#define SEND_IF_COND		8
#define CSD					9
#define CID					10
#define SET_BLOCKLEN		16
#define READ_SINGLE_BLOCK	17
#define WRITE_BLOCK			24
#define SD_SEND_OP_COND		41
#define SCR					51
#define APP_CMD				55
#define READ_OCR			58
#define CRC_ON_OFF			59


#define CMD0				0
#define CMD1				1
#define CMD8				8
#define CMD9				9		// CSD : card Specification Data
#define CMD10				10		// CID : card identification data
#define CMD12				12
#define CMD16				16
#define CMD17				17
#define CMD18				18
#define CMD23				23
#define CMD24				24
#define CMD25				25
#define CMD41			    41
#define CMD51			    51		// SCR : 
#define CMD55				55
#define CMD58				58


#define cmdSDC_CSD()		Microsd_SendCmd( CMD9, 0x00000000 )
#define cmdSDC_CID()		Microsd_SendCmd( CMD10, 0x00000000 )
#define cmdSDC_STP_TRX()	Microsd_SendCmd( CMD12, 0x00000000 )
#define cmdSDC_ACMD()		Microsd_SendCmd( CMD55, 0x00000000 )


#define R1_IDLE_STATE		0x01
#define R1_READY_STATE		0x00
#define START_TOKEN 		0xFE

#define R1_RESPONSE_SIZE	1
#define R3_RESPONSE_SIZE	4
#define R7_RESPONSE_SIZE	4

/* Card type flags (MMC_GET_TYPE) */
#define CT_MMC		0x01		/* MMC ver 3 */
#define CT_SD1		0x02		/* SD ver 1 */
#define CT_SD2		0x04		/* SD ver 2 */
#define CT_SDC		(CT_SD1|CT_SD2)	/* SD */
#define CT_BLOCK	0x08		/* Block addressing */

#define G_POLY7  			0x89   // x^7 + x^3 + 1
#define G_POLY16 			0x1021 // (x^16) + x^12 + x^5 + 1

typedef struct {
	unsigned int	SpiBaseAddr;   // Base Address of the SPI component
	unsigned char	Status;        // microSD status, use SD_STATUS_*_MASK
} Microsd;

struct card_info {
    unsigned char      state;      	// State of card
#define CARD_STATE_DET   (1<<0) 	// b'00000001' - Card detected
#define CARD_STATE_INI   (1<<1) 	// b'00000010' - Card initialized
    unsigned char      type;       	// high nybble - card type - low nybble - capacity
#define CARD_TYPE_MM     (1<<5) 	// b'0010xxxx' - MM Card
#define CARD_TYPE_SD     (1<<4) 	// b'0001xxxx' - SD Card
#define CARD_TYPE_HC     (1<<0) 	// b'xxxx0001' - High Capacity card if bit set
    unsigned char      version;    	// card version - based on card type
#define CARD_VERSD_10    0x00;  	// SD card - 1.00-1.01
#define CARD_VERSD_11    0x01;  	//         - 1.10
#define CARD_VERSD_20    0x02   	//         - 2.00
#define CARD_VERMM_10    0x00   	// MM card - 1.0-1.2
#define CARD_VERMM_14    0x01   	//         - 1.4
#define CARD_VERMM_20    0x02   	//         - 2.0-2.2
#define CARD_VERMM_30    0x03   	//         - 3.1-3.3
#define CARD_VERMM_40    0x04   	//         - 4.0-4.1
    unsigned int       nblock;		// Number of 512 byte blocks on card - use to calculate size
    unsigned int       blocks;		// Number of 512 byte blocks on card - use to calculate size
    unsigned int       csize;		// Number of 512 byte blocks on card - use to calculate size
    //unsigned int       volt;		// Voltage range bits from OCR
    unsigned char      manid;		// Manufacturer ID
    unsigned char      appid[2];	// OEM/Application ID
    unsigned char      name[6];		// Product Name
    unsigned char      rev;		// Product Revision      
    unsigned int		year;		// Product Date - year
    unsigned char		month; 	 	// Product Date - month
    unsigned int		serial;     // Product serial number - can use to detect card change
    unsigned short	oem;
    unsigned char		speed;
    unsigned int		sector;
    unsigned char		ocr[4];
    unsigned char		CardType;
};
struct card_info cInfo;

unsigned char	CardType,				// Card type flag //
				CardInfo[16+16+4+4];	// CSD(16), CID(16), OCR(4), SCR(4) //

unsigned char init_sdc(void);
//inline unsigned char cek_sdc();
unsigned char cek_status_sdc(void);
void SD_kirimCmd(unsigned char* cmd);
unsigned char respon_SDcmd(unsigned char* rspn, int length);
unsigned char cek_versi2_sdc();
unsigned char Microsd_SendCmd(unsigned char cmd_idx, unsigned int arg);
unsigned char Microsd_crc7(unsigned char *MessBuff);
unsigned short Microsd_crc16(unsigned char *MessBuff, unsigned int len);
unsigned char SD_WriteCmd(unsigned char* cmd);
unsigned char Microsd_SendCmd_nocrc(unsigned char cmd_idx, unsigned int arg);
unsigned char sdc_read(unsigned char *data, unsigned int almt, int len);
#endif
