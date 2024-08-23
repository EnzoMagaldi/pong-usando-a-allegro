#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "game.h"
#include "menu.h"

#define SCREEN_WIDTH 1000
#define SCREEN_HEIGHT 500

typedef struct Record {
   char winner[50];
   int leftScore;
   int rightScore;
   struct Record *next;
} Record;

Record *record_head = NULL;

// Função para adicionar um registro à lista de resultados
void add_record(const char *winner, int leftScore, int rightScore);

// Função para salvar resultados em um arquivo
void save_results_to_file();

// Função para exibir os resultados gravados na tela
void show_records(ALLEGRO_FONT *font);