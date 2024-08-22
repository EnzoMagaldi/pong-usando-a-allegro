#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "pong.h"

Node* create_node(char *label) {
    Node *node = (Node*)malloc(sizeof(Node));
    node->label = label;
    node->left = NULL;
    node->right = NULL;
    return node;
}

void draw_button(ALLEGRO_FONT *font, char *label, float x, float y, bool selected) {
    if (selected) {
        al_draw_filled_rectangle(x - 50, y - 20, x + 50, y + 20, al_map_rgb(64, 64, 64)); // Cor de fundo para botão selecionado
        al_draw_text(font, al_map_rgb(255, 255, 0), x, y, ALLEGRO_ALIGN_CENTER, label); // Texto amarelo
    } else {
        al_draw_filled_rectangle(x - 50, y - 20, x + 50, y + 20, al_map_rgb(64, 64, 64)); // Cor de fundo para botão não selecionado
        al_draw_text(font, al_map_rgb(200, 200, 0), x, y, ALLEGRO_ALIGN_CENTER, label); // Texto preto
    }
}

void draw_menu(ALLEGRO_FONT *font, Node *root, Node *selected_node) {
    float x = SCREEN_WIDTH / 2.0;
    float y = SCREEN_HEIGHT / 2.0;
    draw_button(font, root->label, x, y, root == selected_node);
    draw_button(font, root->right->label, x, y + 50, root->right == selected_node);
}

void initBall(Ball *ball, bool towardsRight) {
    ball->x = SCREEN_WIDTH / 2.0 - BALL_SIZE / 2.0;
    ball->y = SCREEN_HEIGHT / 2.0 - BALL_SIZE / 2.0;
    ball->dx = (towardsRight ? 1 : -1) * BALL_SPEED;
    ball->dy = 0;
    ball->moving = true;
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
    if (ball->moving) {
        ball->x += ball->dx;
        ball->y += ball->dy;
        if (ball->y <= 0 || ball->y >= SCREEN_HEIGHT - BALL_SIZE) {
            ball->dy = -ball->dy;
        }
        if (ball->x <= leftPaddle->x + PADDLE_WIDTH && ball->y + BALL_SIZE >= leftPaddle->y && ball->y <= leftPaddle->y + PADDLE_HEIGHT) {
            ball->dx = BALL_SPEED;
            float hitPos = (ball->y + BALL_SIZE / 2) - (leftPaddle->y + PADDLE_HEIGHT / 2);
            ball->dy = hitPos / (PADDLE_HEIGHT / 2) * BALL_SPEED;
            if (ball->dy > BALL_SPEED) ball->dy = BALL_SPEED;
            if (ball->dy < -BALL_SPEED) ball->dy = -BALL_SPEED;
        }
        if (ball->x + BALL_SIZE >= rightPaddle->x && ball->y + BALL_SIZE >= rightPaddle->y && ball->y <= rightPaddle->y + PADDLE_HEIGHT) {
            ball->dx = -BALL_SPEED;
            float hitPos = (ball->y + BALL_SIZE / 2) - (rightPaddle->y + PADDLE_HEIGHT / 2);
            ball->dy = hitPos / (PADDLE_HEIGHT / 2) * BALL_SPEED;
            if (ball->dy > BALL_SPEED) ball->dy = BALL_SPEED;
            if (ball->dy < -BALL_SPEED) ball->dy = -BALL_SPEED;
        }
        if (ball->x < 0) {
            rightPaddle->score++;
            initBall(ball, true);
        } else if (ball->x > SCREEN_WIDTH) {
            leftPaddle->score++;
            initBall(ball, false);
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
    ALLEGRO_COLOR borderColor = al_map_rgb(255, 255, 255);
    int borderThickness = 10;
    al_draw_filled_rectangle(0, 0, borderThickness, SCREEN_HEIGHT, borderColor);
    al_draw_filled_rectangle(SCREEN_WIDTH - borderThickness, 0, SCREEN_WIDTH, SCREEN_HEIGHT, borderColor);
}

void start_game() {
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
            al_clear_to_color(al_map_rgb(128,128,128)); // Fundo preto para o jogo

            drawPaddle(&leftPaddle);
            drawPaddle(&rightPaddle);
            drawBall(&ball);
            drawBorders();
            al_flip_display();
        }
    }

    // Liberação de recursos
    al_destroy_timer(timer);
    al_destroy_display(display);
    al_destroy_event_queue(event_queue);
}

