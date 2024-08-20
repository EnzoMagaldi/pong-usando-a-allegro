#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <stdlib.h>
#include <math.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define PADDLE_WIDTH 10
#define PADDLE_HEIGHT 100
#define BALL_SIZE 10
#define PADDLE_SPEED 6
#define BALL_SPEED 8

typedef struct {
    float x, y;
    float dx, dy;
} Ball;

typedef struct {
    float x, y;
    int score;
    bool up, down;
} Paddle;

void initBall(Ball *ball, bool towardsRight) {
    float angle = ((rand() % 45) + 1) * (M_PI / 180); // Ângulo entre 0° e 45°
    ball->dx = BALL_SPEED * cos(angle) * (towardsRight ? 1 : -1);
    ball->dy = BALL_SPEED * sin(angle) * (rand() % 2 == 0 ? 1 : -1);
    ball->x = SCREEN_WIDTH / 2.0 - BALL_SIZE / 2.0; // Posiciona no meio no eixo X
    ball->y = SCREEN_HEIGHT / 2.0 - BALL_SIZE / 2.0; // Posiciona no meio no eixo Y
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

void updateBall(Ball *ball, Paddle *leftPaddle, Paddle *rightPaddle) {
    ball->x += ball->dx;
    ball->y += ball->dy;

    // Colisão com paredes superior e inferior
    if (ball->y <= 0 || ball->y >= SCREEN_HEIGHT - BALL_SIZE) {
        ball->dy = -ball->dy;
    }

    // Verifica colisão com o paddle esquerdo
    if (ball->x <= leftPaddle->x + PADDLE_WIDTH && ball->y + BALL_SIZE >= leftPaddle->y && ball->y <= leftPaddle->y + PADDLE_HEIGHT) {
        // Reflete a bola para a direita
        ball->dx = fabs(BALL_SPEED * cos(M_PI / 4));  // Direção para a direita

        // Ajusta a direção vertical com base na posição da colisão
        float hitPos = (ball->y + BALL_SIZE / 2) - leftPaddle->y;

        if (hitPos < PADDLE_HEIGHT / 2.0) {
            ball->dy = -fabs(BALL_SPEED * sin(M_PI / 4)); // Ângulo de 45° para cima
        } else {
            ball->dy = fabs(BALL_SPEED * sin(M_PI / 4));  // Ângulo de 45° para baixo
        }
    }

    // Verifica colisão com o paddle direito
    if (ball->x + BALL_SIZE >= rightPaddle->x && ball->y + BALL_SIZE >= rightPaddle->y && ball->y <= rightPaddle->y + PADDLE_HEIGHT) {
        // Reflete a bola para a esquerda
        ball->dx = -fabs(BALL_SPEED * cos(M_PI / 4)); // Direção para a esquerda

        // Ajusta a direção vertical com base na posição da colisão
        float hitPos = (ball->y + BALL_SIZE / 2) - rightPaddle->y;

        if (hitPos < PADDLE_HEIGHT / 2.0) {
            ball->dy = -fabs(BALL_SPEED * sin(M_PI / 4)); // Ângulo de 45° para cima
        } else {
            ball->dy = fabs(BALL_SPEED * sin(M_PI / 4));  // Ângulo de 45° para baixo
        }
    }

    // Pontuação e respawn da bola
    if (ball->x < 0) {
        rightPaddle->score++;
        initBall(ball, true);  // Bola respawna em direção ao jogador da direita
    } else if (ball->x > SCREEN_WIDTH) {
        leftPaddle->score++;
        initBall(ball, false);  // Bola respawna em direção ao jogador da esquerda
    }
}

void drawPaddle(Paddle *paddle) {
    al_draw_filled_rectangle(paddle->x, paddle->y, paddle->x + PADDLE_WIDTH, paddle->y + PADDLE_HEIGHT, al_map_rgb(255, 255, 255));
}

void drawBall(Ball *ball) {
    al_draw_filled_rectangle(ball->x, ball->y, ball->x + BALL_SIZE, ball->y + BALL_SIZE, al_map_rgb(255, 255, 255));
}

int main() {
    ALLEGRO_DISPLAY *display = NULL;
    ALLEGRO_EVENT_QUEUE *event_queue = NULL;
    ALLEGRO_TIMER *timer = NULL;
    bool done = false;
    bool redraw = true;

    // Inicialização do Allegro
    al_init();
    al_install_keyboard();
    al_init_primitives_addon();

    // Inicialização da tela
    display = al_create_display(SCREEN_WIDTH, SCREEN_HEIGHT);
    event_queue = al_create_event_queue();
    timer = al_create_timer(1.0 / 60);

    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_timer_event_source(timer));
    al_register_event_source(event_queue, al_get_keyboard_event_source());

    // Inicializar objetos do jogo
    Ball ball;
    Paddle leftPaddle, rightPaddle;

    srand(time(NULL));
    initBall(&ball, true);  // Começa com a bola indo para a direita
    initPaddle(&leftPaddle, 50, SCREEN_HEIGHT / 2.0 - PADDLE_HEIGHT / 2.0);
    initPaddle(&rightPaddle, SCREEN_WIDTH - 50 - PADDLE_WIDTH, SCREEN_HEIGHT / 2.0 - PADDLE_HEIGHT / 2.0);

    al_start_timer(timer);

    while (!done) {
        ALLEGRO_EVENT event;
        al_wait_for_event(event_queue, &event);

        if (event.type == ALLEGRO_EVENT_TIMER) {
            movePaddle(&leftPaddle);
            movePaddle(&rightPaddle);
            updateBall(&ball, &leftPaddle, &rightPaddle);

            redraw = true;
        } else if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            done = true;
        } else if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
            if (event.keyboard.keycode == ALLEGRO_KEY_W) {
                leftPaddle.up = true;
            } else if (event.keyboard.keycode == ALLEGRO_KEY_S) {
                leftPaddle.down = true;
            } else if (event.keyboard.keycode == ALLEGRO_KEY_UP) {
                rightPaddle.up = true;
            } else if (event.keyboard.keycode == ALLEGRO_KEY_DOWN) {
                rightPaddle.down = true;
            }
        } else if (event.type == ALLEGRO_EVENT_KEY_UP) {
            if (event.keyboard.keycode == ALLEGRO_KEY_W) {
                leftPaddle.up = false;
            } else if (event.keyboard.keycode == ALLEGRO_KEY_S) {
                leftPaddle.down = false;
            } else if (event.keyboard.keycode == ALLEGRO_KEY_UP) {
                rightPaddle.up = false;
            } else if (event.keyboard.keycode == ALLEGRO_KEY_DOWN) {
                rightPaddle.down = false;
            }
        }

        if (redraw && al_is_event_queue_empty(event_queue)) {
            redraw = false;

            al_clear_to_color(al_map_rgb(0, 0, 0));

            drawPaddle(&leftPaddle);
            drawPaddle(&rightPaddle);
            drawBall(&ball);

            al_flip_display();
        }
    }

    // Liberação de recursos
    al_destroy_timer(timer);
    al_destroy_display(display);
    al_destroy_event_queue(event_queue);

    return 0;
}
