
#include "FreeRTOS.h"
#include "lpc23xx.h"
#include "spi_ssp.h"
//#include "adc/ad7708.h"

// pada santer 
//   SPI : -
//	 SSP0: SD card
//	 SSP1: ADC

#if 0
void init_spi0()	{
	S0SPCR = SPI_MSTR;
}

char spiPut (char valueIn)	{
  SPI_SPDR = valueIn;

  while (!(SPI_SPSR & SPI_SPIF));
  return SPI_SPDR;
}
#endif

// untuk SD Card

init_ssp0_sdc()		{
	
}

void init_ssp0(unsigned char waktu)	{
	char i;   
	volatile char dummy;   
	portENTER_CRITICAL();
	
	SSP0CR1	 = 0;
	//SSP1CR0	 = SSP_SCR2 | SSP_CPHA | SSP_CPOL | SSP_DDS8;
	SSP0CR0	 = SSP_DDS8;
	
	if (waktu == SCK_KILAT)
		SSP0CPSR = 6;		// range : 2-254 >> 10MHz (bisa 20-25MHz SDC)
	else
		SSP0CPSR = 200;		// range : 2-254 >> 300kHz (bisa 20-25MHz SDC)
		
	for (i=0; i<FIFO_SSP; i++)	{
		dummy = SSP0DR;
	}
	
	// enable master
	SSP0CR1 = SSP_SSE;
	portEXIT_CRITICAL();
}

void SSP0Send( unsigned char buf, short Length )   {
    short i;   
    //for ( i = 0; i < Length; i++ )	{   
		// as long as TNF bit is set (TxFIFO is not full), I can always transmit //
	
		while ( (SSP0SR & SSP_TNF) != SSP_TNF);
		SSP0DR = buf; 
		//buf++;
		// Wait until the Busy bit is cleared //
		while ( (SSP0SR & SSP_BSY) == SSP_BSY);   

	//}
	return;   
}

unsigned char SSP0Receive( void )	{   
	short i;   
	unsigned char x;
   
	//for ( i = 0; i < Length; i++ )	{   
		/* As long as Receive FIFO is not empty, I can always receive. */   
		/* since it's a loopback test, clock is shared for both TX and RX,  
		no need to write dummy byte to get clock to get the data */   
		/* if it's a peer-to-peer communication, SSPDR needs to be written  
		before a read can take place. */   
		//SSP1DR = 0xFF;   
		while ( !(SSP0SR & SSP_RNE) );
		x = SSP0DR;
	//	buf++;   
	//}   
	return x;
}  

unsigned char SSP0byte(unsigned char cmd)	{
	SSP0Send(cmd, 1);
	return SSP0Receive();
}

// untuk ADC_AD7708
void init_ssp1()	{
	char i;   
	volatile char dummy;   
	
	portENTER_CRITICAL();
	SSP1CR1	 = 0;
	//SSP1CR0	 = SSP_SCR2 | SSP_CPHA | SSP_CPOL | SSP_DDS8;
	SSP1CR0	 = SSP_CPHA | SSP_CPOL | SSP_DDS8;
	SSP1CPSR = 8;		// range : 2-254
	//SSP1IMSC = 0;
	
	for (i=0; i<FIFO_SSP; i++)	{
		dummy = SSP1DR;
	}
	
	// enable master
	SSP1CR1 = SSP_SSE;
	portEXIT_CRITICAL();
}

void SSP1Send( unsigned char buf, short Length )   {
    short i;   
    //for ( i = 0; i < Length; i++ )	{   
		// as long as TNF bit is set (TxFIFO is not full), I can always transmit //
	
		while ( (SSP1SR & SSP_TNF) != SSP_TNF);
		SSP1DR = buf; 
		//buf++;
		// Wait until the Busy bit is cleared //
		while ( (SSP1SR & SSP_BSY) == SSP_BSY);   

	//}
	return;   
}

unsigned char SSP1Receive( void )	{   
	short i;   
	unsigned char x;
   
	//for ( i = 0; i < Length; i++ )	{   
		/* As long as Receive FIFO is not empty, I can always receive. */   
		/* since it's a loopback test, clock is shared for both TX and RX,  
		no need to write dummy byte to get clock to get the data */   
		/* if it's a peer-to-peer communication, SSPDR needs to be written  
		before a read can take place. */   
		//SSP1DR = 0xFF;   
		while ( !(SSP1SR & SSP_RNE) );   
		x = SSP1DR;   
	//	buf++;   
	//}   
	return x;
}   

void SSP1Terima( short Length )	{   
	short i;   
	unsigned char x[10];
	char y;
   
	for ( i = 0; i < Length; i++ )	{   
		/* As long as Receive FIFO is not empty, I can always receive. */   
		/* since it's a loopback test, clock is shared for both TX and RX,  
		no need to write dummy byte to get clock to get the data */   
		/* if it's a peer-to-peer communication, SSPDR needs to be written  
		before a read can take place. */   
		//SSP1DR = 0xFF;   
		while ( !(SSP1SR & SSP_RNE) );   
		x[i] = SSP1DR;
		//qsprintf("%02x ", y);
		//buf++;   
	}
	uprintf("isi : %02x \r\n", x[0]);
	return;
}   
