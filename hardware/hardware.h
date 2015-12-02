
#ifndef __HARDWARE__
#define __HARDWARE__

#include "FreeRTOS.h"
#include "task.h"
#include "lpc23xx.h"
#include "cpu_setup.h"
#include <time.h>

void setup_hardware();
void init_hardware();
void gpio_init();
void gpio_int_init();
int init_konter_onoff(unsigned int kanale, unsigned char status);

void rtcWrite(struct tm *newTime);

#define PORT2_INPUT(kanal)	(FIO2PIN & kanal) ? 1 : 0;
#define PORT1_INPUT(kanal)	(FIO1PIN & kanal) ? 1 : 0;
#define PORT0_INPUT(kanal)	(FIO0PIN & kanal) ? 1 : 0;

#ifdef BOARD_SANTER
	#ifdef BOARD_SANTER_v1_0
		#define uC	"NXP LPC 2387"
	
		#ifdef PAKAI_LED_UTAMA
			#define LED_UTAMA	BIT(18)
			
			#define setup_led_utama()	do {	\
						FIO1DIR |= LED_UTAMA;	\
						FIO1CLR = LED_UTAMA;	\
					} while(0)
			#define toogle_led_utama()	FIO1PIN ^= LED_UTAMA;
		#endif
		
		#ifdef PAKAI_RTC
			#define setup_rtc()	do {				\
						PCONP |= BIT(9);			\
						RTC_CCR  = BIT(4);			\
						RTC_AMR  = 0;				\
						RTC_CIIR = 0;				\
					} while(0)
			#define start_rtc()	do {				\
						RTC_CCR &= ~RTC_CCR_CTCRST;	\
						RTC_CCR |= RTC_CCR_CLKEN;	\
					} while(0)
			//
		#endif
		
		#ifdef PAKAI_RELAY
			#define RLY_1	BIT(0)			/* P1 */
			#define RLY_2	BIT(1)
			#define RLY_3	BIT(4)			
			#define RLY_4	BIT(8)	
			#define RLY_5	BIT(9)
			#define RLY_6	BIT(10)			
			#define RLY_7	BIT(14)
			#define RLY_8	BIT(15)			
			
			#define cRelay1()		FIO1CLR|= RLY_1
			#define cRelay2()		FIO1CLR = RLY_2
			#define cRelay3()		FIO1CLR = RLY_3
			#define cRelay4()		FIO1CLR = RLY_4
			#define cRelay5()		FIO1CLR = RLY_5
			#define cRelay6()		FIO1CLR = RLY_6
			#define cRelay7()		FIO1CLR = RLY_7
			#define cRelay8()		FIO1CLR = RLY_8
			
			#define sRelay1()		FIO1SET = RLY_1
			#define sRelay2()		FIO1SET = RLY_2
			#define sRelay3()		FIO1SET = RLY_3
			#define sRelay4()		FIO1SET = RLY_4
			#define sRelay5()		FIO1SET = RLY_5
			#define sRelay6()		FIO1SET = RLY_6
			#define sRelay7()		FIO1SET = RLY_7
			#define sRelay8()		FIO1SET = RLY_8

			#define setup_relay()		do {	\
						FIO1DIR |= RLY_1;	\
						FIO1CLR = RLY_1;	\
					} while(0)	
			#define toogle_relay_satu()	FIO1PIN ^= RLY_1;
		#endif
		
		#ifdef PAKAI_SHELL
		//#if 1
			#define mainTX_ENABLE	( ( unsigned long ) 0x00010 )
			#define mainRX_ENABLE	( ( unsigned long ) 0x00040 )
			
			#define mainCOM_TEST_PRIORITY		( tskIDLE_PRIORITY + 2 )
			#define mainCOM_TEST_BAUD_RATE		( ( unsigned long ) 115200 )
			//#define mainCOM_TEST_BAUD_RATE		( ( unsigned long ) 9600 )
			
			#define setup_serial0_P0()	do {				\
						PINSEL0 |= mainTX_ENABLE | mainRX_ENABLE;	\
					} while(0)
		#endif
		
		#ifdef PAKAI_SERIAL_3
			#ifdef PAKAI_SERIAL_3_P4
				#define TXDE3	BIT(17)		// P1.17
				#define RXDE3	BIT(29)		// P1.29 --> Pull down ke GND
				
				#define enaTX3_485()		FIO1SET |= TXDE3;
				#define disTX3_485()		FIO1CLR |= TXDE3;
				#define enaRX3_485()		FIO1CLR |= RXDE3;
				
				#define setup_gpio_485_3()	do		{	\
								FIO1DIR |= TXDE3;		\
								FIO1DIR |= RXDE3;		\
							} while (0)
				
				//	TXD3: P4.28, RXD3: P4.29
				#define setup_serial3_P4()	do 	{	\
											PCONP |= BIT(25);	\
											PCLKSEL1 &= ~(BIT(18) | BIT(19));	\
											PCLKSEL1 |= BIT(18);	\
											PINSEL9 |= (BIT(27) | BIT(26) | BIT(25) | BIT(24));	\
											setup_gpio_485_3();		\
							} while(0)
						// PCONP uart2, pclk=cclk, 

				#define init_serial3_P4()	do {				\
							} while(0)
				
			#endif
		#endif
		
		#ifdef PAKAI_SERIAL_2
			#ifdef PAKAI_SERIAL_2_P0
			
				#define TXDE2	BIT(25)		// P3.25
				#define RXDE2	BIT(12)		// --> Pull down ke GND
				
				#define enaTX2_485()		FIO3SET = TXDE2;
				#define disTX2_485()		FIO3CLR = TXDE2;
				#define enaRX2_485()		FIO2CLR = RXDE2;
				
				#define setup_gpio_485_2()	do		{	\
								FIO3DIR = TXDE2;		\
								FIO2DIR = RXDE2;		\
							} while (0)
				
				//	TXD2: P0.10, RXD2: P0.11
				#define setup_serial2_P0()	do 	{	\		
											PCONP |= BIT(24);	\
											PCLKSEL1 &= ~(BIT(16) | BIT(17));	\
											PCLKSEL1 |= BIT(16);	\
											PINSEL0 &= ~(BIT(23) | BIT(22) | BIT(21) | BIT(20));	\
											PINSEL0 |= (BIT(20) | BIT(22));	\
											setup_gpio_485_2();		\
							} while(0)
						// PCONP uart2, pclk=cclk, 

				#define init_serial2_P0()	do {				\
							} while(0)
				
			#endif
		#endif
		
		#ifdef PAKAI_SERIAL_1
			#ifdef PAKAI_SERIAL_1_P2
				// TX1: P2.0, RX1: P2.1
				#define setup_serial1_P2	do	{									\
											PCONP	|= BIT(4);						\
											PCLKSEL0 &= ~(BIT(8) | BIT(9));			\
											PCLKSEL0 |= BIT(8);						\
											PINSEL4 &= ~(BIT(3) | BIT(2) | BIT(1) | BIT(0));	\
											PINSEL4 |= (BIT(3) | BIT(1));			\
				} while (0)
				
			#endif
		#endif

		#ifdef PAKAI_SPI_SSP0		// SDCard
			#define CS_SDC			21
			#define cs_SDCard()		FIO1CLR |= BIT(CS_SDC)
			#define uncs_SDCard()	FIO1SET |= BIT(CS_SDC)
			#define setup_spi_ssp0()	do {	\
						PCONP		|= BIT(21);	\
						PINSEL3		|= BIT(9)  | BIT(8);	\
						PINSEL3		|= BIT(11) | BIT(10);	\
						PINSEL3		|= BIT(15) | BIT(14);	\
						PINSEL3		|= BIT(17) | BIT(16);	\
					} while(0)
					// PCONP default AKTIF
					// PINSEL SPI : SCK0 | SCK0 | MISO0 | MOSI0	
		#endif
		
		#ifdef PAKAI_SPI_SSP1		// ADC AD7708
			#define setup_spi_ssp1()	do {	\
						PCONP		|= BIT(10);	\
						PINSEL0		|= BIT(13);	\
						PINSEL0		|= BIT(15);	\
						PINSEL0		|= BIT(17);	\
						PINSEL0		|= BIT(19);	\
					} while(0)
					// PCONP default AKTIF
					// PINSEL SPI : SSEL1 | SCK1 | MISO1 | MOSI1
					// set ssel supaya tidak cs & uncs spi (auto)
		#endif	
		
		#ifdef PAKAI_SPI1_P0
			#define setup_spi1_p0()	do {	\
						PCONP		|= BIT(8);				\
						PINSEL0 	|= BIT(15) | BIT(17) | BIT(19);	\
						PCLKSEL0	|= BIT(16) | BIT(17);	\	
					} while(0)
					// PCONP defaultnya AKTIF
					// PINSEL SPI : SCK1 | MISO1 | MOSI1
					// PCLK/8 ~ 133ns
		#endif

		#ifdef PAKAI_ADC_7708
			#define RDY_AD7708		11		// P211
			#define setup_eint1()	do {			\
						PINSEL4		|= BIT(22);		\
					} while(0)

		#endif
		
		#ifdef PAKAI_SDCARD
			#define INS_SDC			2
			#define setup_sdc()		do	{					\
						PINSEL4		&= ~(BIT(5) | BIT(4));	\
						FIO2DIR 	&= ~(BIT(INS_SDC));		\
						PINMODE4	|= (BIT(5) | BIT(4));	\
					} while (0)
			
		#endif
	
		#define iKonter_1		BIT(5) 	// P25, konter_1
		#define iKonter_2		BIT(6)	// P26
		#define iKonter_3		BIT(7)	// P27
		#define iKonter_4		BIT(8)	// P28
		#define iKonter_5		BIT(9)	// P29
		
		#define iKonter_6		BIT(24)	// P024, konter_6
		#define iKonter_7		BIT(25)	// P025
		#define iKonter_8		BIT(26)	// P026
		#define iKonter_9		BIT(27)	// P027
		#define iKonter_10		BIT(28)	// P028

		
		#define POWER_5V		BIT(19)	// P019
		#define POWER_24V		BIT(28)	// P128
		#define POWER_2n5V		BIT(29)	// P029

		#define setup_power()		do {	\
					FIO0DIR  |= POWER_5V;		\
					FIO1DIR  |= POWER_24V;		\
				} while(0)	

	#endif
#endif

#endif
