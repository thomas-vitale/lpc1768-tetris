#include "../GLCD/GLCD.h"
#include "tetris.h"

extern void copy_block(volatile uint8_t dest[L][L], const volatile uint8_t src[L][L]);
const int color_block[8] = {Black, Cyan, Blue, Orange, Yellow, Green, Purple, Red};



void Tetris_you_loose(){
	
	GUI_Text(10, 150, (uint8_t *)"       LOSE        ", Red, White);
	
}

void Tetris_pause(void)
{

    if (game == 1)
    {
        if (is_paused == 0)
        {
            is_paused = 1;
            GUI_Text(10, 150, (uint8_t *)"       PAUSE       ", Red, White);
        }
        else
        {
            is_paused = 0;
            Tetris_draw_map_pause();
        }
    }

}

void Tetris_start_window(void)
{

	LCD_DrawRectangle(5, 5, 165, 315, White, 0, 5); // 150 px horizontal - 300 vertical each square 15x15 px
	Tetris_draw_new_map();
	score_text();
	update_score();
}

void Tetris_draw_map(void)
{
    int r, c;
    volatile int act_color;
    volatile uint16_t x0_lcd, y0_lcd;
			
	
    for (r = 0; r < ROWS; r++)
    {
        for (c = 0; c < COLS; c++)
        {
            x0_lcd = c * W + 10;
            y0_lcd = r * W + 10;
            act_color = board[r][c];

            if (act_color > 0 && act_color <= 7) 
            {
                 LCD_DrawRectangle(x0_lcd + 1, y0_lcd + 1, x0_lcd + W - 1, y0_lcd + W- 1, color_block[act_color], 1, 0);
            }
						else{
						LCD_DrawRectangle(x0_lcd, y0_lcd, x0_lcd + W, y0_lcd + W, Black, 1, 0);
						}

        }
    }
		
}


void Tetris_draw_new_map(void)
{
		int r, c;
		int color[7] = {Black, Cyan, Blue, Orange, Yellow, Green, Red};
		LCD_DrawRectangle(10, 10, 160, 310, Black, 1, 0);
		volatile int act_color;
		volatile uint16_t x0_lcd, y0_lcd;
		for (r = 0; r < ROWS; r++)
		{
			for (c = 0; c < COLS; c++)
			{
				board[r][c] = 0;
			}
		}
		
}

void Tetris_update_board()
{

	Draw_Active_Piece(x0_old, y0_old, 0);

	if (isnewpiece)
	{
		copy_block(active, new_active);
	}

	
	Draw_Active_Piece(x0_coor, y0_coor, 1);
}

void Draw_Active_Piece(int x, int y, int draw_color_mode)
{
    volatile int r, c;
    volatile int col_idx;
    volatile uint16_t x_lcd, y_lcd;

    

    for (r = 0; r < 4; r++)
    {
        for (c = 0; c < 4; c++)
        {
            if (active[r][c] != 0)
            {
                x_lcd = (x + c) * W + 10;
                y_lcd = (y + r) * W + 10;

                if (draw_color_mode == 1)
                {
                    col_idx = active[r][c];
                    if (col_idx > 8) col_idx = 7; 
                    LCD_DrawRectangle(x_lcd + 1, y_lcd + 1, x_lcd + W - 1, y_lcd + W -1 , color_block[col_idx], 1, 0);
                }
                else
                {
                    // Mode 0: Cancella (disegna nero)
                    LCD_DrawRectangle(x_lcd + 1, y_lcd + 1, x_lcd + W - 1, y_lcd + W -1 , Black, 1, 0);
                }
            }
        }
    }
}

void score_text(){
	GUI_Text(170, 20, (uint8_t *) "Record: ", White, Black);
	GUI_Text(170, 60, (uint8_t *) "Score: ", White, Black);
	GUI_Text(170, 100, (uint8_t *) "Total L: ", White, Black);
	GUI_Text(170, 240, (uint8_t *) "Next: ", White, Black);
	
}

void update_score(){
	enable_rit = 0;

	uint8_t numtostring[20];
	
	my_itoa(high_score, numtostring);
	GUI_Text(170, 35, (uint8_t *) numtostring, White, Black);
	my_itoa(score, numtostring);
	GUI_Text(170, 75, (uint8_t *) numtostring, White, Black);
	my_itoa(total_lines, numtostring);
	GUI_Text(170, 115, (uint8_t *) numtostring, White, Black);
	enable_rit = 1;
}


void my_itoa(int value, uint8_t* str) {
    int i = 0;
    int k = 0;
    int temp = value;
    uint8_t reverse[20]; 

    do {
        reverse[k++] = (temp % 10) + '0'; 
        temp /= 10;
    } while (temp > 0);

    while (k < 7) {
        reverse[k++] = '0';
    }

    while (k > 0) {
        str[i++] = reverse[--k];
    }
    
    str[i] = '\0';
}

void Tetris_draw_map_pause(){

    int r, c;
    volatile int act_color;
    volatile uint16_t x0_lcd, y0_lcd;
			
		enable_rit = 0;
	
    for (r = 9; r < 11; r++)
    {
        for (c = 0; c < COLS; c++)
        {
            x0_lcd = c * W + 10;
            y0_lcd = r * W + 10;
            act_color = board[r][c];

            if (act_color > 0 && act_color <= 7) 
            {
                 LCD_DrawRectangle(x0_lcd + 1, y0_lcd + 1, x0_lcd + W - 1, y0_lcd + W- 1, color_block[act_color], 1, 0);
            }
						else{
						LCD_DrawRectangle(x0_lcd, y0_lcd, x0_lcd + W, y0_lcd + W, Black, 1, 0);
						}

        }
    }
		enable_rit = 1;
}

void next_block(){
	
  int next_block = (int)LFSR_step(current_state, taps)%7;
	LCD_DrawRectangle(175, 260, 235, 320, Black, 1, 0);
	copy_block(active, tetrominos_block[next_block][current_rot]);
	Draw_Active_Piece(11, 17 , 1);
	

}


