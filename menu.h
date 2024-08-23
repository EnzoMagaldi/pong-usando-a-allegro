#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

typedef struct Node {
char *label;
struct Node *up;
struct Node *down;
} Node;

// Funções do menu
Node* create_node(char *label, Node *up, Node *down);

void draw_button(ALLEGRO_FONT *font, char *label, float x, float y, bool selected);

void draw_menu(ALLEGRO_FONT *font, Node *current_node);