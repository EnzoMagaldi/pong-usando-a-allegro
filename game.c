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
void initBall(Ball *ball, bool towardsRight) {
   // Posiciona a bola no centro da tela
   ball->x = SCREEN_WIDTH / 2.0 - BALL_SIZE / 2.0;
   ball->y = SCREEN_HEIGHT / 2.0 - BALL_SIZE / 2.0;
   ball->dx = (towardsRight ? 1 : -1) * BALL_SPEED; // Movimento horizontal inicial
   ball->dy = 0; // Movimento vertical inicial
   ball->moving = true; // A bola deve começar a se mover
}

void initPaddle(Paddle *paddle, float x, float y) {
   paddle->x = x;
   paddle->y = y;
   paddle->score = 0;
   paddle->up = false;
   paddle->down = false;
}

void movePaddle(Paddle *paddle) {
   if (paddle->up) {
      paddle->y -= PADDLE_SPEED;
      if (paddle->y < 0) paddle->y = 0;
   } else if (paddle->down) {
      paddle->y += PADDLE_SPEED;
      if (paddle->y > SCREEN_HEIGHT - PADDLE_HEIGHT) paddle->y = SCREEN_HEIGHT - PADDLE_HEIGHT;
   }
}

void updateBall(Ball *ball, Paddle *leftPaddle, Paddle *rightPaddle, bool *game_over) {
   if (ball->moving) {
      ball->x += ball->dx;
      ball->y += ball->dy;

      // Colisão com paredes superior e inferior
      if (ball->y <= 0 || ball->y >= SCREEN_HEIGHT - BALL_SIZE) {
         ball->dy = -ball->dy;
      }

      // Verifica colisão com o paddle esquerdo
      if (ball->x <= leftPaddle->x + PADDLE_WIDTH && ball->y + BALL_SIZE >= leftPaddle->y && ball->y <= leftPaddle->y + PADDLE_HEIGHT) {
         ball->dx = BALL_SPEED;  // Movimento horizontal para a direita
         float hitPos = (ball->y + BALL_SIZE / 2) - (leftPaddle->y + PADDLE_HEIGHT / 2);
         ball->dy = hitPos / (PADDLE_HEIGHT / 2) * BALL_SPEED;
      }

      // Verifica colisão com o paddle direito
      if (ball->x + BALL_SIZE >= rightPaddle->x && ball->y + BALL_SIZE >= rightPaddle->y && ball->y <= rightPaddle->y + PADDLE_HEIGHT) {
         ball->dx = -BALL_SPEED;  // Movimento horizontal para a esquerda
         float hitPos = (ball->y + BALL_SIZE / 2) - (rightPaddle->y + PADDLE_HEIGHT / 2);
         ball->dy = hitPos / (PADDLE_HEIGHT / 2) * BALL_SPEED;
      }

      // Pontuação e respawn da bola
      if (ball->x < 0) {
         rightPaddle->score++;
         if (rightPaddle->score >= 5) {
               *game_over = true;
         } else {
               initBall(ball, true);
         }
      } else if (ball->x > SCREEN_WIDTH) {
         leftPaddle->score++;
         if (leftPaddle->score >= 5) {
               *game_over = true;
         } else {
               initBall(ball, false);
         }
      }
   }
}

void drawPaddle(Paddle *paddle) {
   al_draw_filled_rectangle(paddle->x, paddle->y, paddle->x + PADDLE_WIDTH, paddle->y + PADDLE_HEIGHT, al_map_rgb(255, 255, 255));
}

void drawBall(Ball *ball) {
   al_draw_filled_rectangle(ball->x, ball->y, ball->x + BALL_SIZE, ball->y + BALL_SIZE, al_map_rgb(255, 255, 255));
}

void drawBorders() {
   // Cor da borda
   ALLEGRO_COLOR borderColor = al_map_rgb(255, 0, 0);

   // Espessura da borda
   int borderThickness = 10;

   // Desenha a borda esquerda
   al_draw_filled_rectangle(0, 0, borderThickness, SCREEN_HEIGHT, borderColor);

   // Desenha a borda direita
   al_draw_filled_rectangle(SCREEN_WIDTH - borderThickness, 0, SCREEN_WIDTH, SCREEN_HEIGHT, borderColor);
}

// Mostra o placar na tela
void drawScore(ALLEGRO_FONT *font, Paddle *leftPaddle, Paddle *rightPaddle) {
   char scoreText[100];
   sprintf(scoreText, "%d", leftPaddle->score);
   al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_WIDTH / 4, 20, ALLEGRO_ALIGN_CENTER, scoreText);
   sprintf(scoreText, "%d", rightPaddle->score);
   al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_WIDTH * 3 / 4, 20, ALLEGRO_ALIGN_CENTER, scoreText);
}

// Função principal do jogo
void start_game(Ball *ball, Paddle *leftPaddle, Paddle *rightPaddle, ALLEGRO_EVENT_QUEUE *event_queue, ALLEGRO_TIMER *timer, ALLEGRO_FONT *font, bool *restart_game) {
   bool done = false;
   bool redraw = true;
   bool game_over = false;
   char winner[50]; // Variável para armazenar o vencedor

   initBall(ball, true);  // Começa com a bola indo para a direita
   initPaddle(leftPaddle, 50, SCREEN_HEIGHT / 2.0 - PADDLE_HEIGHT / 2.0);
   initPaddle(rightPaddle, SCREEN_WIDTH - 50 - PADDLE_WIDTH, SCREEN_HEIGHT / 2.0 - PADDLE_HEIGHT / 2.0);

   al_start_timer(timer);

   while (!done) {
      ALLEGRO_EVENT event;
      al_wait_for_event(event_queue, &event);

      if (event.type == ALLEGRO_EVENT_TIMER) {
         movePaddle(leftPaddle);
         movePaddle(rightPaddle);
         updateBall(ball, leftPaddle, rightPaddle, &game_over);

         redraw = true;
      } else if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
         done = true;
         *restart_game = false;
      } else if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
         switch (event.keyboard.keycode) {
               case ALLEGRO_KEY_W:
                  leftPaddle->up = true;
                  break;
               case ALLEGRO_KEY_S:
                  leftPaddle->down = true;
                  break;
               case ALLEGRO_KEY_UP:
                  rightPaddle->up = true;
                  break;
               case ALLEGRO_KEY_DOWN:
                  rightPaddle->down = true;
                  break;
         }
      } else if (event.type == ALLEGRO_EVENT_KEY_UP) {
         switch (event.keyboard.keycode) {
               case ALLEGRO_KEY_W:
                  leftPaddle->up = false;
                  break;
               case ALLEGRO_KEY_S:
                  leftPaddle->down = false;
                  break;
               case ALLEGRO_KEY_UP:
                  rightPaddle->up = false;
                  break;
               case ALLEGRO_KEY_DOWN:
                  rightPaddle->down = false;
                  break;
               case ALLEGRO_KEY_ESCAPE:
                  done = true;
                  *restart_game = false;
                  break;
         }
      }

      if (redraw && al_is_event_queue_empty(event_queue)) {
         al_clear_to_color(al_map_rgb(0, 0, 0));

         drawPaddle(leftPaddle);
         drawPaddle(rightPaddle);
         drawBall(ball);
         drawScore(font, leftPaddle, rightPaddle);
         drawBorders(); // Desenha as bordas

         al_flip_display();
         redraw = false;
      }

      if (game_over) {
         done = true;
         if (leftPaddle->score >= 5) {
               sprintf(winner, "Jogador 1 venceu!");
         } else {
               sprintf(winner, "Jogador 2 venceu!");
         }

         // Adiciona o resultado à lista de registros
         add_record(winner, leftPaddle->score, rightPaddle->score);

         // Exibe a mensagem do vencedor
         al_clear_to_color(al_map_rgb(0, 0, 0));
         al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 - 50, ALLEGRO_ALIGN_CENTER, winner);
         al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 + 50, ALLEGRO_ALIGN_CENTER, "Pressione ESC para sair");
         al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 + 100, ALLEGRO_ALIGN_CENTER, "Pressione ENTER para reiniciar");
         al_flip_display();

         bool wait_input = true;
         while (wait_input) {
               ALLEGRO_EVENT event;
               al_wait_for_event(event_queue, &event);

               if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
                  switch (event.keyboard.keycode) {
                     case ALLEGRO_KEY_ENTER:
                           wait_input = false;
                           *restart_game = true;
                           break;
                     case ALLEGRO_KEY_ESCAPE:
                           wait_input = false;
                           *restart_game = false;
                           break;
                  }
               }
         }

         // Salva os resultados em um arquivo
         save_results_to_file();
      }
   }
}