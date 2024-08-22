#ifndef PONH_H
#define PONG_H

#include <allegro5/allegro_font.h>

// Definições de constantes
#define SCREEN_WIDTH 1000
#define SCREEN_HEIGHT 500
#define PADDLE_WIDTH 10
#define PADDLE_HEIGHT 50
#define BALL_SIZE 10
#define BALL_SPEED 8
#define PADDLE_SPEED 6

// Estruturas
typedef struct Ball {
    float x, y, dx, dy;
    bool moving;
} Ball;

typedef struct Paddle {
    float x, y;
    int score;
    bool up, down;
} Paddle;

typedef struct Node {
    char *label;
    struct Node *left;
    struct Node *right;
} Node;

// Funções
Node* create_node(char *label);
void draw_button(ALLEGRO_FONT *font, char *label, float x, float y, bool selected);
void draw_menu(ALLEGRO_FONT *font, Node *root, Node *selected_node);
void initBall(Ball *ball, bool towardsRight);
void initPaddle(Paddle *paddle, float x, float y);
void movePaddle(Paddle *paddle);
void updateBall(Ball *ball, Paddle *leftPaddle, Paddle *rightPaddle);
void drawPaddle(Paddle *paddle);
void drawBall(Ball *ball);
void drawBorders();
void start_game();

#endif
