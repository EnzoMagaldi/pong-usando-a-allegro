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
#define PADDLE_WIDTH 10
#define PADDLE_HEIGHT 100
#define BALL_SIZE 10
#define PADDLE_SPEED 6
#define BALL_SPEED 10
#define MAX_ANGLE (45 * (M_PI / 180))  // Ângulo máximo de 45 graus em radianos

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

typedef struct Node {
char *label;
struct Node *up;
struct Node *down;
} Node;

typedef struct {
float x, y;
float dx, dy;
bool moving;  // Flag para verificar se a bola está se movendo
} Ball;

typedef struct {
float x, y;
int score;
bool up, down;
} Paddle;


// Funções do jogo
void initBall(Ball *ball, bool towardsRight);

void initPaddle(Paddle *paddle, float x, float y);

void movePaddle(Paddle *paddle);

void updateBall(Ball *ball, Paddle *leftPaddle, Paddle *rightPaddle, bool *game_over);

void drawPaddle(Paddle *paddle);

void drawBall(Ball *ball);

void drawBorders();

// Mostra o placar na tela
void drawScore(ALLEGRO_FONT *font, Paddle *leftPaddle, Paddle *rightPaddle);

// Função principal do jogo
void start_game(Ball *ball, Paddle *leftPaddle, Paddle *rightPaddle, ALLEGRO_EVENT_QUEUE *event_queue, ALLEGRO_TIMER *timer, ALLEGRO_FONT *font, bool *restart_game);