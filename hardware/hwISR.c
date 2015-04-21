
#include <stdlib.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "hardware.h"
#include "lpc23xx.h"
#include "monita.h"

void timer1_ISR_Wrapper( void ) __attribute__ ((naked));
void timer1_ISR_Handler( void );
void gpio_ISR_Wrapper( void ) __attribute__ ((naked));
void gpio_ISR_Handler( void );

//extern char strmb[];
//extern char outmb[];

extern struct t2_konter konter;
//extern unsigned char status_konter[];
extern struct t_adc adc;

void timer1_ISR_Wrapper( void )	{
	/* Save the context of the interrupted task. */
	portSAVE_CONTEXT();
	
	/* Call the handler.  This must be a separate function from the wrapper
	to ensure the correct stack frame is set up. */
	__asm volatile( "bl timer1_ISR_Handler" );
	//timer1_ISR_Handler();

	/* Restore the context of whichever task is going to run next. */
	portRESTORE_CONTEXT();
}

void timer1_ISR_Handler( void )	{
	konter.ovflow++;

	T1IR = TxIR_MR0_Interrupt;   // clear interrupt by writing an IR-bit

	/* Clear the ISR in the VIC. */
	VICVectAddr = 0;
}

void gpio_ISR_Wrapper( void )	{
	// Save the context of the interrupted task. //
	portSAVE_CONTEXT();
	
	// Call the handler.  This must be a separate function from the wrapper
	// to ensure the correct stack frame is set up.
	
#if 0
	/* 11 Sept 2009, jika GPIO hanya untuk status on off */
	gpio_ISR_KONTROL();
#endif
	__asm volatile( "bl gpio_ISR_Handler" );
	//gpio_ISR_Handler();

	/* Restore the context of whichever task is going to run next. */
	portRESTORE_CONTEXT();
}

void gpio_ISR_Handler( void )	{
	int t=0, zz=0;
	unsigned int new_period;
	struct t_env *env2;
	env2 = (char *) ALMT_ENV;
	//int xx = env2->kalib[i].status; 
	
	konter.global_hit++;
	new_period = T1TC;
	//cek sumber

	#if 0
	if (IO0_INT_STAT_F & iKonter_10)	{
		t = 9;	zz = status_konter[t];
		if (zz==sRPM) {
			set_konter_rpm(t, new_period);
		}
		#if 0
		if (zz==1) {
			set_konter_onoff(t, 0);
		}
		#endif
		IO0_INT_CLR = iKonter_10;
	}
	#endif
	if (IO0_INT_STAT_R & iKonter_10) {
		t = 9; zz = env2->kalib[t].status; 
		if ((zz==sPROP) || (zz==sRPM) || (zz==sRPM_RH) || (zz==sFLOWx)) {
			set_konter_rpm(t, new_period);
		} 
		#if 0
		else if (zz==sONOFF) {
			set_konter_onoff(t, 1);
		#ifdef PAKAI_PILIHAN_FLOW
		} else if (zz==100)	{
			//FIO0PIN ^= LED_UTAMA;
			set_konter_flow_pilih(t, konter.t_konter[t-1].onoff);
		#endif
		}
		#endif
		IO0_INT_CLR = iKonter_10;
	} 
	
	#if 0
	if (IO0_INT_STAT_F & iKonter_9) {
		t = 8;	zz = status_konter[t];
		if (zz==0) {
			set_konter_rpm(t, new_period);
		} else if (zz==1 || zz==3) {
			set_konter_onoff(t, 1);		
		#ifdef PAKAI_PILIHAN_FLOW
		} else if (zz==202)	{
			set_konter_flow_pilih(t, zz);
		#endif
		}
		IO0_INT_CLR = iKonter_9;
	}
	#endif
	if (IO0_INT_STAT_R & iKonter_9)	{
		t = 8;	zz = env2->kalib[t].status; 
		if ((zz==sPROP) || (zz==sRPM) || (zz==sRPM_RH) || (zz==sFLOWx)) {
			set_konter_rpm(t, new_period);
		}
		#if 0
		if (zz==1) {
			set_konter_onoff(t, 0);
		#ifdef PAKAI_PILIHAN_FLOW
		} else if (zz==3)	{
			set_konter_onoff(t, 0);
		#endif
		}
		#endif
		IO0_INT_CLR = iKonter_9;
	}

	#if 0
	if (IO0_INT_STAT_F & iKonter_8) {		// buat pulsa
		t = 7; zz = status_konter[t];
		if (zz==0) {
			set_konter_rpm(t, new_period);
		} else if (zz==1) {
			set_konter_onoff(t, 1);
		#ifdef PAKAI_PUSHBUTTON
		} else if (zz==2) {
			if (debound[t]==0) {
				debound[t] = DELAY_DEBOUND;
				#ifdef PAKAI_RELAY
					toogle_selenoid(t+1);
				#endif
			}
		#endif
		#ifdef PAKAI_PILIHAN_FLOW
		} else if (zz==100) {
			set_konter_flow_pilih(t, konter.t_konter[t-1].onoff);
		} else if (zz==201)	{
			set_konter_onoff(t, 1);
		#endif
		}
		IO0_INT_CLR = iKonter_8;
	} 
	#endif
	if (IO0_INT_STAT_R & iKonter_8)	{
		t = 7;  zz = env2->kalib[t].status; 
		if ((zz==sPROP) || (zz==sRPM) || (zz==sRPM_RH) || (zz==sFLOWx)) {
			set_konter_rpm(t, new_period);
		}
		#if 0
		if (zz==1) {
			set_konter_onoff(t, 0);
		#ifdef PAKAI_PILIHAN_FLOW
		} else if (zz==3)	{
			set_konter_onoff(t, 0);
		} else if (zz==4)	{
			set_konter_onoff(t, 0);
		} else if (zz==201)	{
			set_konter_onoff(t, 0);
		#endif
		}
		#endif
		IO0_INT_CLR = iKonter_8;
	}

	#if 0
	if (IO0_INT_STAT_F & iKonter_7) {
		t = 6; zz = status_konter[t];
		if (zz==0) {
			set_konter_rpm(t, new_period);
		} else if (zz==1) {
			set_konter_onoff(t, 1);
		#ifdef PAKAI_PUSHBUTTON
		} else if (zz==2) {
			if (debound[t]==0) {
				debound[t] = DELAY_DEBOUND;
				#ifdef PAKAI_RELAY
					toogle_selenoid(t+1);
				#endif
			}
		#endif
		#ifdef PAKAI_PILIHAN_FLOW
		} else if (zz==3)	{
			set_konter_onoff(t, 1);
		} else if (zz==4)	{
			set_konter_onoff(t, 1);
		} else if (zz==201)	{
			set_konter_onoff(t, 1);
		#endif
		}
		IO0_INT_CLR = iKonter_7;
	} 
	#endif
	if (IO0_INT_STAT_R & iKonter_7)	{
		t = 6; zz = env2->kalib[t].status; 
		if ((zz==sPROP) || (zz==sRPM) || (zz==sRPM_RH) || (zz==sFLOWx)) {
			set_konter_rpm(t, new_period);
		}
		#if 0
		if (zz==1) {
			set_konter_onoff(t, 0);
		#ifdef PAKAI_PILIHAN_FLOW
		} else if (zz==3)	{
			set_konter_onoff(t, 0);
		} else if (zz==4)	{
			set_konter_onoff(t, 0);
		} else if (zz==201)	{
			set_konter_onoff(t, 0);
		#endif
		}
		#endif
		IO0_INT_CLR = iKonter_7;
	}

	#if 0
	if (IO0_INT_STAT_F & iKonter_6) {
		t = 5;  zz = status_konter[t];
		if (zz==0) {
			set_konter_rpm(t, new_period);
		} else if (zz==1) {
			set_konter_onoff(t, 1);
		#ifdef PAKAI_PUSHBUTTON
		} else if (zz==2) {
			if (debound[t]==0) {
				debound[t] = DELAY_DEBOUND;
				#ifdef PAKAI_RELAY
					toogle_selenoid(t+1);
				#endif
			}
		#endif
		#ifdef PAKAI_PILIHAN_FLOW
		} else if (zz==3)	{
			set_konter_onoff(t, 1);
		} else if (zz==202)	{
			set_konter_flow_pilih(t, zz);
		#endif
		}
		IO0_INT_CLR = iKonter_6;
	} 
	#endif
	if (IO0_INT_STAT_R & iKonter_6)	{
		t = 5;  zz = env2->kalib[t].status; 
		if ((zz==sPROP) || (zz==sRPM) || (zz==sRPM_RH) || (zz==sFLOWx)) {
			set_konter_rpm(t, new_period);
		}
		#if 0
		if (zz==1) {
			set_konter_onoff(t, 0);
		#ifdef PAKAI_PILIHAN_FLOW
		} else if (zz==100) {
			set_konter_flow_pilih(t, konter.t_konter[t-1].onoff);
		#endif
		}
		#endif
		IO0_INT_CLR = iKonter_6;
	}


	#if 0
	if (IO2_INT_STAT_F & iKonter_5)	{
		t = 4;  zz = status_konter[t];
		if (zz==0) {
			set_konter_rpm(t, new_period);
		} else if (zz==1 || zz==3) {
			set_konter_onoff(t, 1);
		#ifdef PAKAI_PUSHBUTTON
		} else if (zz==2) {
			set_konter_onoff(t, 1);
			if (debound[t]==0) {
				debound[t] = DELAY_DEBOUND;
				#ifdef PAKAI_RELAY
					toogle_selenoid(t+1);
				#endif
			}
		#endif
		#ifdef PAKAI_PILIHAN_FLOW
		} else if (zz==100) {
			set_konter_flow_pilih(t, konter.t_konter[t-1].onoff);
		} else if (zz==201)	{
			set_konter_onoff(t, 1);
		#endif
		}
		//set_konter(t, new_period);
		IO2_INT_CLR = iKonter_5;
	}
	#endif
	if (IO2_INT_STAT_R & iKonter_5)	{
		t = 4;	zz = env2->kalib[t].status; 
		if ((zz==sPROP) || (zz==sRPM) || (zz==sRPM_RH) || (zz==sFLOWx)) {
			set_konter_rpm(t, new_period);
		}
		#if 0
		if (status_konter[t]==1) {
			set_konter_onoff(t, 0);
		#ifdef PAKAI_PILIHAN_FLOW
		} else if (zz==3)	{
			set_konter_onoff(t, 0);
		} else if (zz==4)	{
			set_konter_onoff(t, 0);
		} else if (zz==201)	{
			set_konter_onoff(t, 0);
		#endif
		}
		#endif
		IO2_INT_CLR = iKonter_5;
	}
	
	#if 0
	if (IO2_INT_STAT_F & iKonter_4)	{
		t = 3;  zz = status_konter[t];
		//set_konter_rpm(t, new_period);
		if (zz==0) {
			set_konter_rpm(t, new_period);
		} else if (zz==1) {
			set_konter_onoff(t, 1);
		#ifdef PAKAI_PUSHBUTTON
		} else if (zz==2) {
			if (debound[t]==0) {
				debound[t] = DELAY_DEBOUND;
				#ifdef PAKAI_RELAY
					toogle_selenoid(t+1);
				#endif
			}
		#endif
		#ifdef PAKAI_PILIHAN_FLOW
		} else if (zz==3)	{
			set_konter_onoff(t, 1);
		} else if (zz==4)	{
			set_konter_onoff(t, 1);
		} else if (zz==201)	{
			set_konter_onoff(t, 1);
		#endif
		}		
		IO2_INT_CLR = iKonter_4;
	}
	#endif
	if (IO2_INT_STAT_R & iKonter_4)	{
		t = 3;	zz = env2->kalib[t].status; 

		if ((zz==sPROP) || (zz==sRPM) || (zz==sRPM_RH) || (zz==sFLOWx)) {
			set_konter_rpm(t, new_period);
		}
		#if 0
		if (zz==1) {
			set_konter_onoff(t, 0);
		#ifdef PAKAI_PILIHAN_FLOW
		} else if (zz==3)	{
			set_konter_onoff(t, 0);
		} else if (zz==4)	{
			set_konter_onoff(t, 0);
		} else if (zz==201)	{
			set_konter_onoff(t, 0);
		#endif
		}
		#endif
		IO2_INT_CLR = iKonter_4;
	}
	
	if (IO2_INT_STAT_R & iKonter_3)	{
		t = 2; zz = env2->kalib[t].status; 
		if ((zz==sPROP) || (zz==sRPM) || (zz==sRPM_RH) || (zz==sFLOWx)) {
			set_konter_rpm(t, new_period);
		}
		#if 1
		else if (zz==sONOFF) {
			set_konter_onoff(t, 0);
		#ifdef PAKAI_PILIHAN_FLOW
		} else if (zz==3)	{
			set_konter_onoff(t, 0);
		} else if (zz==4)	{
			set_konter_onoff(t, 0);
		} else if (zz==201)	{
			set_konter_onoff(t, 0);
		#endif
		}
		#endif
		IO2_INT_CLR = iKonter_3;
	}
	#if 0
	if (IO2_INT_STAT_F & iKonter_3)	{
		t = 2; //zz = status_konter[t];
		zz = env2->kalib[t].status; 
		//set_konter(t, new_period);
		if (zz==sRPM) {
			set_konter_rpm(t, new_period);
		} else if (zz==sONOFF) {
			set_konter_onoff(t, 1);
		#ifdef PAKAI_PUSHBUTTON
		} else if (zz==sPUSHBUTTON) {
			if (debound[t]==0) {
				debound[t] = DELAY_DEBOUND;
				#ifdef PAKAI_RELAY
					toogle_selenoid(t+1);
				#endif
			}
		#endif
		#ifdef PAKAI_PILIHAN_FLOW
		} else if (zz==3)	{
			set_konter_onoff(t, 1);
		} else if (zz==202)	{
			set_konter_flow_pilih(t, zz);
		#endif
		}
		IO2_INT_CLR = iKonter_3;
	}
	#endif

	if (IO2_INT_STAT_R & iKonter_2)	{
		t = 1; zz = env2->kalib[t].status; 
		if ((zz==sPROP) || (zz==sRPM) || (zz==sRPM_RH) || (zz==sFLOWx)) {
			set_konter_rpm(t, new_period);
		}
		#if 0
		else if (zz==sONOFF) {
			set_konter_onoff(t, 1);
		#ifdef PAKAI_PUSHBUTTON
		} else if (zz==sPUSHBUTTON) {
			if (debound[t]==0) {
				debound[t] = DELAY_DEBOUND;
				#ifdef PAKAI_RELAY
					toogle_selenoid(t+1);
				#endif
			}
		#endif
		#ifdef PAKAI_PILIHAN_FLOW
		} else if (zz==100) {
			set_konter_flow_pilih(t, konter.t_konter[t-1].onoff);
		} else if (zz==201)	{
			set_konter_onoff(t, 1);
		#endif
		}
		#endif
		IO2_INT_CLR = iKonter_2;
	}
	#if 0
	if (IO2_INT_STAT_F & iKonter_2)	{
		t = 1;  zz = status_konter[t];
		if (zz==1) {
			set_konter_onoff(t, 0);
		#ifdef PAKAI_PILIHAN_FLOW
		} else if (zz==3)	{
			set_konter_onoff(t, 0);
		} else if (zz==4)	{
			set_konter_onoff(t, 0);
		} else if (zz==201)	{
			set_konter_onoff(t, 0);
		#endif
		}
		IO2_INT_CLR = iKonter_2;
	}
	#endif
	
	if (IO2_INT_STAT_R & iKonter_1)	{
		t = 0;	zz = env2->kalib[t].status; 
		if ((zz==sPROP) || (zz==sRPM) || (zz==sRPM_RH) || (zz==sFLOWx)) {
			set_konter_rpm(t, new_period);
		}
		#if 0
		else if (zz==sONOFF) {
			set_konter_onoff(t, 1);
		#ifdef PAKAI_PUSHBUTTON
		} else if (zz==sPUSHBUTTON) {
			if (debound[t]==0) {
				debound[t] = DELAY_DEBOUND;
				#ifdef PAKAI_RELAY
					toogle_selenoid(t+1);
				#endif
			}
		#endif
		#ifdef PAKAI_PILIHAN_FLOW
		} else if (zz==sFLOW1)	{
			set_konter_onoff(t, 1);
		} else if (zz==sFLOW2)	{
			set_konter_onoff(t, 1);
		} else if (zz==ssFLOW2)	{
			set_konter_onoff(t, 1);
		#endif
		}
		#endif
		IO2_INT_CLR = iKonter_1;
	}
	#if 0
	if (IO2_INT_STAT_F & iKonter_1)	{
		t = 0;	zz = status_konter[t];
		if (zz==1) {
			set_konter_onoff(t, 0);
		#ifdef PAKAI_PILIHAN_FLOW
		} else if (zz==3)	{
			set_konter_onoff(t, 0);
		} else if (zz==4)	{
			set_konter_onoff(t, 0);
		} else if (zz==201)	{
			set_konter_onoff(t, 0);
		#endif
		}
		IO2_INT_CLR = iKonter_1;
	}
	#endif


	#ifdef PAKAI_ADC_7708x
		if (IO2_INT_STAT_F & RDY_AD7708)	{
			ambil_data_ad7708();
			
			IO2_INT_CLR = RDY_AD7708;
		}
	#endif
	// Clear the ISR in the VIC. //
	VICVectAddr = 0;
}
