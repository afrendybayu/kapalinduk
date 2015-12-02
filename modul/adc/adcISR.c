
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "hardware.h"
#include "lpc23xx.h"
#include "monita.h"

#ifdef PAKAI_ADC_7708

void adc_ISR_Wrapper( void ) __attribute__ ((naked));
void adc_ISR_Handler( void );


void adc_ISR_Wrapper( void )	{
	// Save the context of the interrupted task. //
	portSAVE_CONTEXT();
	
	// Call the handler.  This must be a separate function from the wrapper
	// to ensure the correct stack frame is set up.

	__asm volatile( "bl adc_ISR_Handler" );

	/* Restore the context of whichever task is going to run next. */
	portRESTORE_CONTEXT();
}

void adc_ISR_Handler( void )	{
	ada_data_ad7708();
}	

#endif
