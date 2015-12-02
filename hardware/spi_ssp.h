
#ifndef __SPI_SSP__
#define __SPI_SSP__

#include "lpc23xx.h"

#define SPI_BitE	BIT(2)
#define SPI_CPHA	BIT(3)
#define SPI_CPOL	BIT(4)
#define SPI_MSTR	BIT(5)
#define SPI_LSBF	BIT(6)
#define SPI_SPIE	BIT(7)

#define SSP_LBM		BIT(0)
#define SSP_SSE		BIT(1)
#define SSP_SLAVE	BIT(2)
#define SSP_SOD		BIT(3)
#define SSP_CPOL	BIT(6)
#define SSP_CPHA	BIT(7)

#define SSP_DDS8	0x07
#define SSP_DDS16	0x0F

#define SSP_FR_TI	(1<<4)
#define SSP_FR_MW	(2<<4)

#define SSP_SCR0	(0<<8)
#define SSP_SCR1	(1<<8)
#define SSP_SCR2	(2<<8)

#define SSP_TFE		BIT(0)
#define SSP_TNF		BIT(1)
#define SSP_RNE		BIT(2)
#define SSP_RFF		BIT(3)
#define SSP_BSY		BIT(4)

#define FIFO_SSP	8
#define SCK_LAMA	200
#define SCK_KILAT	3

void init_spi0();
char spiPut (char valueIn);

void init_ssp0(unsigned char waktu);
void SSP0Send( unsigned char buf, short Length );
unsigned char SSP0Receive( void );
unsigned char SSP0byte(unsigned char cmd);

void init_ssp1();
void SSP1Send( unsigned char buf, short Length );
unsigned char SSP1Receive(void);
void SSP1Terima ( short Length );

#endif
