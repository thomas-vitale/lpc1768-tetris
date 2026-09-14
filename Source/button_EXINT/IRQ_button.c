#include "button.h"
#include "LPC17xx.h"

extern volatile int down;
extern volatile int down_1;

void EINT0_IRQHandler (void)	  	/* INT0														 */
{		

	LPC_SC->EXTINT &= (1 << 0);     /* clear pending interrupt         */
}



void EINT1_IRQHandler(void) /* KEY1 */
{
    down_1 = 1;                 
    NVIC_DisableIRQ(EINT1_IRQn);
		LPC_PINCON->PINSEL4 &= ~(1 << 22); 
    LPC_SC->EXTINT &= (1 << 1); 
}

void EINT2_IRQHandler (void)      /* KEY2 */
{
                           
    NVIC_DisableIRQ(EINT2_IRQn);        
    down = 1;
    LPC_PINCON->PINSEL4 &= ~(1 << 24);     
    LPC_SC->EXTINT &= (1 << 2);             
}


