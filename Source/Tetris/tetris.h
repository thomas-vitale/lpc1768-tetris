#ifndef __TETRIS_H
#define __TETRIS_H

#include "LPC17xx.h"
#include <stdint.h>

#define ROWS 20
#define COLS 10
#define W    15
#define L    4
#define seed 0x5F

// --- Variabili Esterne ---

// Matrice active per il gioco corrente
extern  uint8_t active[L][L] __attribute__((aligned(4)));
extern  uint8_t new_active[L][L] __attribute__((aligned(4)));

extern volatile uint8_t game;

// Matrice Assets: [7 pezzi][4 rotazioni][4 righe][4 colonne]
extern volatile uint8_t tetrominos_block[7][4][4][4] __attribute__((aligned(4)));

// Mappa statica
extern volatile int board[ROWS][COLS] __attribute__((aligned(4)));

// Coordinate
extern volatile int x0_coor, y0_coor;
extern volatile int x0_old, y0_old;

// --- NUOVE VARIABILI DI STATO ---
extern volatile int current_piece_idx; // Quale pezzo (0-6) è attivo
extern volatile int current_rot;       // Rotazione corrente (0-3)

extern volatile int speedup;
extern volatile int position;
extern volatile int isnewpiece;
extern volatile int todraw;
extern volatile int enable_rit;
extern volatile int removed_lines;
extern volatile int new_piece;
extern volatile int update_screen;
extern volatile int total_lines;


extern volatile int high_score;
extern volatile int score;
extern volatile int is_paused;


extern unsigned char LFSR_step(unsigned char, unsigned char);
extern void copy_block(volatile uint8_t dest[L][L], const volatile uint8_t src[L][L]);

extern unsigned char current_state;
extern unsigned char taps;

// --- Funzioni ---
void Tetris_new_game(void);
void new_tetrominos(void);
void Tetris_rotate_block(void);
void Tetris_next_time(void);
int Tetris_go_down(void);
void Tetris_update_board();
void Tetris_pause();
// Helper e GFX
int checkcollision(int tx, int ty, uint8_t src[L][L]);
void lock_piece(void);
void Tetris_move_piece(int position);
int remove_completed_lines(void);
void Tetris_start_window(void);
void Tetris_draw_new_map(void);
void Tetris_draw_map(void);
void Draw_Active_Piece(int x, int y, int draw_color_mode);
void update_score();
void my_itoa(int value, uint8_t* str);
void Tetris_draw_map_pause(void);
void Tetris_you_loose(void);
void score_text(void);
void next_block();
#endif