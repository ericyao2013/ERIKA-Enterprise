/** 
* @file uwl_timer_pic32.c
* @brief Timer Implementation for PIC32
* @author Christian Nastasi
* @author Daniele Alessandrelli
* @version 0.1
* @date 2010-02-09
*/
#include <hal/uwl_timer_pic32.h>

static void (* volatile isr_callback)(void) = NULL;

int8_t uwl_timer_init(uint32_t period, uint32_t fcy) 
{
	uint32_t pr;

	UWL_TIMER_INTERRUPT_DISABLE();
	UWL_TIMER_REG_CON_CLEAR();
	if (fcy / 1000) {
		fcy = fcy / 1000;	/* Represent the frquency in 10^6 */
	} else if (period / 1000) {
		period = period / 1000; /* Represent the period in 10^-3 */
	} else if ((fcy / 100) && (period / 10)) {
		fcy = fcy / 100;	/* Represent the frquency in 10^5 */
		period = period / 10; 	/* Represent the period in 10^-5 */
	} else if ((fcy / 10) && (period / 100)) {
		fcy = fcy / 10;		/* Represent the frquency in 10^4 */
		period = period / 100; 	/* Represent the period in 10^-4 */
	} else {
		return -1;		/* Unable to represent!*/
	}
	pr = period * fcy;
	if (pr <= 0xFFFF) {
		UWL_TIMER_REG_PERIOD = pr;
	} else if ((pr / 8) <= 0xFFFF) {
		UWL_TIMER_REG_PERIOD = pr / 8;
		UWL_TIMER_REG_CONbits.TCKPS = 1;
	} else if ((pr / 64) <= 0xFFFF) {
		UWL_TIMER_REG_PERIOD = pr / 64;
		UWL_TIMER_REG_CONbits.TCKPS = 2;
	} else if ((pr / 256) <= 0xFFFF) {
		UWL_TIMER_REG_PERIOD = pr / 256;
		UWL_TIMER_REG_CONbits.TCKPS = 3;
	} else {
		return -2;		/* Unable to have this period! */
	}
	UWL_TIMER_INTERRUPT_PRIORITY = 5;
	UWL_TIMER_INTERRUPT_SUBPRIORITY = 0;
	UWL_TIMER_INTERRUPT_FLAG_CLEAR();
	UWL_TIMER_REG_TMR = 0;
	return 1;
}

void uwl_timer_set_isr_callback(void (*func)(void))
{
	isr_callback = func;
} 

COMPILER_ISR(UWL_TIMER_INTERRUPT_NAME)
{
    UWL_TIMER_INTERRUPT_FLAG_CLEAR();
	if (isr_callback != NULL)
		isr_callback();
}

