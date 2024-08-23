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
void add_record(const char *winner, int leftScore, int rightScore) {
   Record *new_record = (Record*)malloc(sizeof(Record));
   strcpy(new_record->winner, winner);
   new_record->leftScore = leftScore;
   new_record->rightScore = rightScore;
   new_record->next = record_head;
   record_head = new_record;
}

// Função para salvar resultados em um arquivo
void save_results_to_file() {
   FILE *file = fopen("results.txt", "w");
   if (!file) {
      fprintf(stderr, "Não foi possível abrir o arquivo de resultados.\n");
      return;
   }

   Record *current = record_head;
   while (current) {
      fprintf(file, "%s - Jogador 1: %d | Jogador 2: %d\n", current->winner, current->leftScore, current->rightScore);
      current = current->next;
   }

   fclose(file);
}

// Função para exibir os resultados gravados na tela
void show_records(ALLEGRO_FONT *font) {
   FILE *file = fopen("results.txt", "r");
   if (!file) {
      fprintf(stderr, "Não foi possível abrir o arquivo de resultados.\n");
      return;
   }

   char line[256];
   int y_offset = 50;

   al_clear_to_color(al_map_rgb(0, 0, 0));

   while (fgets(line, sizeof(line), file)) {
      al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_WIDTH / 2, y_offset, ALLEGRO_ALIGN_CENTER, line);
      y_offset += 30;  // Espaçamento entre linhas
   }

   fclose(file);

   al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_WIDTH / 2, SCREEN_HEIGHT - 50, ALLEGRO_ALIGN_CENTER, "Pressione ESC para voltar ao menu");
   al_flip_display();
}