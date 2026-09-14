#include "tetris.h"
#include "../GLCD/GLCD.h"

// Definizioni variabili di gioco e matrici
uint8_t active[L][L] __attribute__((aligned(4))) = {0};
uint8_t new_active[L][L] __attribute__((aligned(4))) = {0};
volatile int board[ROWS][COLS] = {0};

volatile int x0_coor = 0, y0_coor = 0;
volatile int x0_old = 0, y0_old = 0;
volatile int new_piece = 1;
volatile int speedup = 1;
volatile int isnewpiece = 0;
volatile int todraw = 0;
volatile int removed_lines = 0;
volatile int score = 0;
volatile int high_score = 0;
volatile int is_paused = 0;
volatile int total_lines = 0;
volatile int enable_rit = 0;

unsigned char current_state = seed;
unsigned char taps = seed;

volatile int current_piece_idx = 0;
volatile int current_rot = 0;

volatile uint8_t game = 0;

// Prototipi funzioni locali
void lock_piece(void);
int remove_completed_lines(void);

// Generazione pseudo-casuale tramite LFSR
int rand(void)
{
	int result = 0;
	current_state = LFSR_step(current_state, taps);
	result = (int)current_state;
	return result % 7; 
}

// Inizializzazione nuovo tetramino nella matrice attiva
void new_tetrominos(void)
{
	current_piece_idx = rand();
	current_rot = 0;

	copy_block(new_active, tetrominos_block[current_piece_idx][current_rot]);
	next_block();
	copy_block(active, new_active);
}

// Verifica collisioni con bordi del display e blocchi fissati
int checkcollision(int tx, int ty, uint8_t src[4][4])
{
	int r, c;
	int abs_x, abs_y;
	for (r = 0; r < L; r++)
	{
		for (c = 0; c < L; c++)
		{
			if (src[r][c] != 0)
			{
				abs_x = tx + c;
				abs_y = ty + r;

				if (abs_x < 0 || abs_x >= COLS || abs_y >= ROWS) return 1;
				if (abs_y >= 0 && board[abs_y][abs_x] != 0) return 1;
			}
		}
	}
	return 0;
}

// Reset variabili per nuova partita
void Tetris_new_game(void)
{
	removed_lines = 0;
	total_lines = 0;
	score = 0;
	Tetris_start_window();
	game = 1;
	Tetris_pause();
}

// Funzione principale scandita dal timer (RIT)
void Tetris_next_time(void)
{
	if (game)
	{
		if (new_piece)
		{
			new_tetrominos();
			x0_coor = (COLS / 2) - 2;
			y0_coor = 0;
			x0_old = x0_coor;
			y0_old = y0_coor;
			isnewpiece = 0;

			// Controllo immediato per Game Over
			if (checkcollision(x0_coor, y0_coor, active))
			{
				Tetris_you_loose();
				game = 0;
				high_score = score;
				Tetris_new_game();
			}
			else
				Draw_Active_Piece(x0_coor, y0_coor, 1);
			new_piece = 0;
		}
		else
		{
			if (Tetris_go_down()) new_piece = 1;
		}
	}
}

// Gestione rotazione con controllo collisioni (Wall Kick)
void Tetris_rotate_block(void)
{
	int next_rot = (current_rot + 1) % 4;
	copy_block(new_active, tetrominos_block[current_piece_idx][next_rot]);

	if (checkcollision(x0_coor, y0_coor, new_active) == 0)
	{
		current_rot = next_rot;
		isnewpiece = 1;
		todraw = 1;
	}
	else
	{
		// Tentativo traslazione laterale se rotazione impedita
		if (checkcollision(x0_coor + 1, y0_coor, new_active) == 0)
		{
			x0_coor++;
			current_rot = next_rot;
			isnewpiece = 1;
			todraw = 1;
		}
		else if (checkcollision(x0_coor - 1, y0_coor, new_active) == 0)
		{
			x0_coor--;
			current_rot = next_rot;
			isnewpiece = 1;
			todraw = 1;
		}
	}
}

// Fissa il pezzo corrente nella griglia statica
void lock_piece(void)
{
	int r, c;
	for (r = 0; r < 4; r++)
	{
		for (c = 0; c < 4; c++)
		{
			if (active[r][c] != 0)
			{
				if ((y0_coor + r < ROWS) && (x0_coor + c < COLS))
					board[y0_coor + r][x0_coor + c] = active[r][c];
			}
		}
	}
	score += 10;
	update_score();
}

// Eliminazione righe complete e shift matrice board
int remove_completed_lines(void)
{
	int r, c, k;
	int full, isFull = 0;
	removed_lines = 0;

	for (r = ROWS - 1; r >= 0; r--)
	{
		full = 1;
		for (c = 0; c < COLS; c++)
		{
			if (board[r][c] == 0)
			{
				full = 0;
				break;
			}
		}

		if (full)
		{
			removed_lines++;
			isFull = 1;
			// Shift verso il basso
			for (k = r; k > 0; k--)
			{
				for (c = 0; c < COLS; c++) board[k][c] = board[k - 1][c];
			}
			for (c = 0; c < COLS; c++) board[0][c] = 0;
			r++; 
		}
	}
	
	total_lines += removed_lines;
	
	// Aggiornamento punteggio (Bonus per Tetris)
	if(removed_lines != 0) 
	{
		score += (removed_lines == 4) ? 600 : (removed_lines * 100);
		update_score();
	}
	
	return isFull;
}

// Movimento laterale e gestione drop (soft/hard)
void Tetris_move_piece(int position)
{
	int right = 0;
	
	if (position != 4)
	{
		if (position == 3) right = -1;
		if (position == 2) right = 1;
		if (checkcollision(x0_coor + right, y0_coor, active) == 0)
		{
			x0_coor += right;
			isnewpiece = 0;
			todraw = 1;
		}
	}
	if (position == 4) speedup = 2; // Soft drop
	if (position == 6) // Hard drop
	{
		while (checkcollision(x0_coor, y0_coor + 1, active) == 0)
		{
			y0_coor++;
		}
		isnewpiece = 0;
		todraw = 1;
	}
}

// Discesa automatica del pezzo
int Tetris_go_down()
{
	if (checkcollision(x0_coor, y0_coor +1, active) == 0)
	{
		y0_coor += 1;
		isnewpiece = 0;
		todraw = 1;
		return 0;
	}
	else
	{
		lock_piece();
		if (remove_completed_lines())
			Tetris_draw_map();
		return 1;
	}
}