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
#include "records.h"

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

typedef struct Record {
    char winner[50];
    int leftScore;
    int rightScore;
    struct Record *next;
} Record;

Record *record_head = NULL;

int main(int argc, char **argv) {
    ALLEGRO_DISPLAY *display = NULL;
    ALLEGRO_EVENT_QUEUE *event_queue = NULL;
    ALLEGRO_TIMER *timer = NULL;
    ALLEGRO_FONT *font = NULL;
    bool restart_game = true;

    if (!al_init()) {
        fprintf(stderr, "Falha ao inicializar Allegro.\n");
        return -1;
    }

    display = al_create_display(SCREEN_WIDTH, SCREEN_HEIGHT);
    if (!display) {
        fprintf(stderr, "Falha ao criar display.\n");
        return -1;
    }

    if (!al_init_primitives_addon()) {
        fprintf(stderr, "Falha ao inicializar add-on de primitivas.\n");
        return -1;
    }

    al_install_keyboard();
    al_init_font_addon();
    al_init_ttf_addon();

    font = al_load_font("maine.ttf", 36, 0);
    if (!font) {
        fprintf(stderr, "Falha ao carregar a fonte.\n");
        return -1;
    }

    event_queue = al_create_event_queue();
    timer = al_create_timer(1.0 / 60);

    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_timer_event_source(timer));
    al_register_event_source(event_queue, al_get_keyboard_event_source());

    Node *menu = create_node("Start", NULL, NULL);
    menu->down = create_node("Records", menu, NULL);
    menu->down->down = create_node("Exit", menu->down, NULL);

    Node *selected_node = menu;

    while (restart_game) {
        bool done = false;
        bool redraw = true;

        al_start_timer(timer);

        while (!done) {
            ALLEGRO_EVENT event;
            al_wait_for_event(event_queue, &event);

            if (event.type == ALLEGRO_EVENT_TIMER) {
                redraw = true;
            } else if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
                done = true;
                restart_game = false;
            } else if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
                switch (event.keyboard.keycode) {
                    case ALLEGRO_KEY_DOWN:
                        if (selected_node->down) {
                            selected_node = selected_node->down;
                        }
                        break;
                    case ALLEGRO_KEY_UP:
                        if (selected_node->up) {
                            selected_node = selected_node->up;
                        }
                        break;
                    case ALLEGRO_KEY_ENTER:
                        if (selected_node == menu) {
                            done = true;
                        } else if (selected_node == menu->down) {
                            // Exibe a tela de resultados
                            while (true) {
                                show_records(font);
                                ALLEGRO_EVENT event;
                                al_wait_for_event(event_queue, &event);

                                if (event.type == ALLEGRO_EVENT_KEY_DOWN && event.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
                                    break;  // Volta ao menu principal
                                }
                            }
                            selected_node = menu; // Retorna ao menu principal
                        } else if (selected_node == menu->down->down) {
                            done = true;
                            restart_game = false;
                        }
                        break;
                    case ALLEGRO_KEY_ESCAPE:
                        done = true;
                        restart_game = false;
                        break;
                }
            }

            if (redraw && al_is_event_queue_empty(event_queue)) {
                al_clear_to_color(al_map_rgb(0, 0, 0));
                draw_menu(font, selected_node);
                al_flip_display();
                redraw = false;
            }
        }

        if (restart_game) {
            Ball ball;
            Paddle leftPaddle, rightPaddle;
            start_game(&ball, &leftPaddle, &rightPaddle, event_queue, timer, font, &restart_game);
        }
    }

    al_destroy_font(font);
    al_destroy_timer(timer);
    al_destroy_event_queue(event_queue);
    al_destroy_display(display);

    // Libera a memória da lista de registros
    while (record_head) {
        Record *temp = record_head;
        record_head = record_head->next;
        free(temp);
    }

    return 0;
}
