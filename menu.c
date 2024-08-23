#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define SCREEN_WIDTH 1000
#define SCREEN_HEIGHT 500

typedef struct Node {
char *label;
struct Node *up;
struct Node *down;
} Node;


// Funções do menu
Node* create_node(char *label, Node *up, Node *down) {
   Node *node = (Node*)malloc(sizeof(Node));
   node->label = label;
   node->up = up;
   node->down = down;
   return node;
}

void draw_button(ALLEGRO_FONT *font, char *label, float x, float y, bool selected) {
   if (selected) {
      al_draw_filled_rectangle(x - 50, y - 20, x + 50, y + 20, al_map_rgb(0, 0, 0)); // Cor de fundo para botão selecionado
      al_draw_text(font, al_map_rgb(255, 255, 0), x, y, ALLEGRO_ALIGN_CENTER, label); // Texto branco
   } else {
      al_draw_filled_rectangle(x - 50, y - 20, x + 50, y + 20, al_map_rgb(0, 0, 0)); // Cor de fundo para botão não selecionado
      al_draw_text(font, al_map_rgb(200, 200, 0), x, y, ALLEGRO_ALIGN_CENTER, label); // Texto preto
   }
}

void draw_menu(ALLEGRO_FONT *font, Node *current_node) {
   float x = SCREEN_WIDTH / 2.0;
   float y = SCREEN_HEIGHT / 2.0;
   
   // Encontre o primeiro nó
   Node *first_node = current_node;
   while (first_node->up != NULL) {
      first_node = first_node->up;
   }
   
   // Desenha as opções de menu
   int offset = 0;
   Node *node = first_node;
   while (node) {
      draw_button(font, node->label, x, y + offset, node == current_node);
      offset += 50;  // Espaçamento entre os botões
      node = node->down;
   }
}