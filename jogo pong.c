#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include "pong.h"

int main() {
    ALLEGRO_DISPLAY *display = NULL;
    ALLEGRO_EVENT_QUEUE *event_queue = NULL;
    ALLEGRO_FONT *font = NULL;
    ALLEGRO_TIMER *timer = NULL;
    Node *root = NULL;
    Node *selected_node = NULL;
    bool done = false;
    bool redraw = true;

    // Inicialização do Allegro
    al_init();
    al_install_keyboard();
    al_init_font_addon();
    al_init_ttf_addon();
    al_init_primitives_addon();

    // Inicialização da tela
    display = al_create_display(SCREEN_WIDTH, SCREEN_HEIGHT);
    event_queue = al_create_event_queue();
    timer = al_create_timer(1.0 / 60);
    font = al_load_font("maine.ttf", 36, 0); // Certifique-se de ter o arquivo de fonte adequado

    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_timer_event_source(timer));
    al_register_event_source(event_queue, al_get_keyboard_event_source());

    // Inicializar menu
    root = create_node("Start");
    root->right = create_node("Exit");
    selected_node = root;  // Inicia com o botão "Start" selecionado

    al_start_timer(timer);

    while (!done) {
        ALLEGRO_EVENT event;
        al_wait_for_event(event_queue, &event);

        if (event.type == ALLEGRO_EVENT_TIMER) {
            redraw = true;
        } else if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            done = true;
        } else if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
            if (event.keyboard.keycode == ALLEGRO_KEY_DOWN) {
                selected_node = selected_node->right ? selected_node->right : root;
            } else if (event.keyboard.keycode == ALLEGRO_KEY_UP) {
                selected_node = selected_node == root ? root->right : root;
            } else if (event.keyboard.keycode == ALLEGRO_KEY_SPACE) {
                if (selected_node == root) {
                    start_game();
                }if (selected_node == root->right) {
                    done = true; // Sair do jogo
                }
            }
        }

        if (redraw && al_is_event_queue_empty(event_queue)) {
            redraw = false;
            al_clear_to_color(al_map_rgb(64, 64, 64));
            draw_menu(font, root, selected_node);
            al_flip_display();
        }
    }

    // Liberação de recursos
    al_destroy_timer(timer);
    al_destroy_display(display);
    al_destroy_event_queue(event_queue);
    al_destroy_font(font);

    return 0;
}
