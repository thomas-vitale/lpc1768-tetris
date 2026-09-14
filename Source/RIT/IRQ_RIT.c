/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_RIT.c
** Descriptions:        functions to manage T0 and T1 interrupts
** Correlated files:    RIT.h
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include "LPC17xx.h"
#include "RIT.h"
#include "../Tetris/tetris.h"

/******************************************************************************
** Function name:		RIT_IRQHandler
**
** Descriptions:		REPETITIVE INTERRUPT TIMER handler
******************************************************************************/
void read_input(void);


volatile int down = 0;
volatile int down_1 = 0; 
volatile int counter_step = 0;
volatile int position = 0;

void RIT_IRQHandler(void)
{
		static int old_position;
		
		if(is_paused == 0){
		
		   if(todraw) {
				disable_RIT(); 
        Tetris_update_board();
        todraw = 0;
				y0_old = y0_coor;
				x0_old = x0_coor;
				reset_RIT();
				enable_RIT();
			 }
		if(counter_step >= 40/speedup){
				disable_RIT();
        Tetris_next_time();
        counter_step = 0;
				reset_RIT();
				enable_RIT(); 
    }
		}

    
    if(counter_step % 4 == 1) // Read input every 100 ms 
    {		 disable_RIT();
         read_input();
				 if(is_paused) counter_step=0; 
				 reset_RIT();
				 enable_RIT();
    }
    

    counter_step++;
    LPC_RIT->RICTRL |= 0x1; /* clear interrupt flag */
}

void read_input(void){

    static int old_position = 0; 
    position = 0; // Reset position ogni ciclo

    // --- Polling Joystick (P1.25 - P1.29) ---
    if ((LPC_GPIO1->FIOPIN & (1 << 29)) == 0)      position = 1; // UP
    else if ((LPC_GPIO1->FIOPIN & (1 << 28)) == 0) position = 2; // RIGHT
    else if ((LPC_GPIO1->FIOPIN & (1 << 27)) == 0) position = 3; // LEFT
    else if ((LPC_GPIO1->FIOPIN & (1 << 26)) == 0) position = 4; // DOWN (Fast)
    else if ((LPC_GPIO1->FIOPIN & (1 << 25)) == 0) position = 5; // SELECT
	

    
    if(down_1 >= 1){ 
        if((LPC_GPIO2->FIOPIN & (1<<11)) == 0){  /* KEY1 premuto */
            switch(down_1){                
                case 1:  
                    down_1++; 
                    break;
                default:
                    break;
            }
        }
        else { 
            down_1 = 0; 
						Tetris_pause();
            NVIC_EnableIRQ(EINT1_IRQn); 
            LPC_PINCON->PINSEL4 |= (1 << 22);
        }
    }

    // --- Gestione Button KEY2 (Hard Drop/Action) - P2.12 ---
    if(down >= 1){ 
        if((LPC_GPIO2->FIOPIN & (1<<12)) == 0){  /* KEY2 ancora premuto */
            switch(down){                
                case 1:
                    down++; // Passa a stato 2 (attesa rilascio)
                    break;
                default:
                    break;
            }
        }
        else { /* Rilascio KEY2 rilevato */
            position = 6;
            down = 0;
            NVIC_EnableIRQ(EINT2_IRQn); 
            LPC_PINCON->PINSEL4 |= (1 << 24); 
        }
    }
    //Non leggere l'input del joystick: se il gioco è in pausa, se sta creando un nuovo pezzo o se c'è il soft drop
    if (is_paused == 1 || new_piece == 1 || (position == old_position && position!=4)) return;
    switch (position) {
						case 0: speedup = 1; break;
            case 1: Tetris_rotate_block(); break;
            case 2: Tetris_move_piece(position); break; // Right
            case 3: Tetris_move_piece(position); break; // Left
            case 4: Tetris_move_piece(position); break; // Fast Down
            case 6: Tetris_move_piece(position); break; // Hard Down (KEY2)
    }
    
    old_position = position; // Aggiorna stato precedente
}

/******************************************************************************
** End Of File
******************************************************************************/