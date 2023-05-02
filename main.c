#include "lib.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>
#include <stdbool.h>
#include <stdlib.h>

#define PRINT_SDL_ERROR() \
    (printf("[%d] sdl error: %s\n", __LINE__, SDL_GetError()));

#define SETCLR(r, g, b) (SDL_SetRenderDrawColor(renderer, (r), (g), (b), 255))

void select_color(SDL_Renderer* renderer, int8_t block) {
    switch (block) {
        case Y:
            SETCLR(255, 255, 0);
            break;
        case R:
            SETCLR(255, 0, 0);
            break;
        case L:
            SETCLR(0, 50, 255);
            break;
        case B:
            SETCLR(0, 0, 255);
            break;
        case O:
            SETCLR(255, 50, 0);
            break;
        case G:
            SETCLR(0, 255, 0);
            break;
        case P:
            SETCLR(255, 0, 255);
            break;
        default:
            SETCLR(200, 200, 200);
    }
}

bool try_to_survive(kek_field_t* field, kek_kekro_t* kekro) {
    int tries = 5;
    int prev_x = kekro->x;
    int prev_y = kekro->y;

    while (0 < tries) {
        int collision = kek_kekro_check_collision(kekro, field);
        if (collision == NONE_COLLISION)
            return true;
        if (collision == LEFTRIGHT_COLLISION || collision == BOTTOM_COLLISION)
            kekro->y -= 1;
        if (collision == LEFT_COLLISION)
            kekro->x += 1;
        if (collision == RIGHT_COLLISION)
            kekro->x -= 1;
        tries -= 1;
    }

    kekro->x = prev_x;
    kekro->y = prev_y;

    return false;
}

int main() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        PRINT_SDL_ERROR();
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow(
        "Kekris", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 300, 600,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

    if (window == NULL) {
        PRINT_SDL_ERROR();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(
        window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (renderer == NULL) {
        PRINT_SDL_ERROR();
        return 1;
    }

    srand(SDL_GetTicks());

    kek_kekro_t kekro;
    kek_kekro_select_random(&kekro);
    kekro.x = 3;
    kekro.y = 3;

    kek_field_t kekris_field = {0};

    int fall_delay_ms = 1000;

    int diff_time;
    int next_fall_counter_ms = SDL_GetTicks();

    int screen_w;
    int screen_h;
    SDL_GetWindowSizeInPixels(window, &screen_w, &screen_h);
    SDL_Rect field_rect;
    field_rect.h = screen_h;
    field_rect.w = screen_h / KEKRIS_HEIGHT * KEKRIS_WIDTH;
    field_rect.x = screen_w / 2 - field_rect.w / 2;
    field_rect.y = 0;
    int cell_size_px = field_rect.h / KEKRIS_WIDTH;

    bool running = true;
    SDL_Event ev;

    while (running) {
        SDL_PollEvent(&ev);
        SDL_FlushEvent(SDL_KEYDOWN);

        if (ev.type == SDL_QUIT) {
            running = false;
            break;
        } else if (ev.type == SDL_KEYDOWN) {
            switch (ev.key.keysym.sym) {
                case SDLK_f:
                    kek_kekro_blit(&kekro, &kekris_field);
                    break;
                case SDLK_LEFT:
                    kekro.x -= 1;
                    if (kek_kekro_check_collision(&kekro, &kekris_field) !=
                        NONE_COLLISION)
                        kekro.x += 1;
                    break;
                case SDLK_RIGHT:
                    kekro.x += 1;
                    if (kek_kekro_check_collision(&kekro, &kekris_field) !=
                        NONE_COLLISION)
                        kekro.x -= 1;
                    break;
                case SDLK_UP:
                    kek_kekro_rotate(&kekro, ev.key.keysym.mod == KMOD_LSHIFT);
                    if (!try_to_survive(&kekris_field, &kekro)) {
                        kek_kekro_rotate(&kekro,
                                         ev.key.keysym.mod != KMOD_LSHIFT);
                    }
                    break;
                case SDLK_DOWN:
                    kekro.y += 1;
                    if (kek_kekro_check_collision(&kekro, &kekris_field) !=
                        NONE_COLLISION)
                        kekro.y -= 1;
                    break;
            }
        }

    // const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);

        int prev_x = kekro.x;
        int prev_y = kekro.y;

        diff_time = SDL_GetTicks() - next_fall_counter_ms;
        if (fall_delay_ms <= diff_time && prev_y == kekro.y) {
            next_fall_counter_ms = SDL_GetTicks();
            kekro.y += 1;

            kek_collision_t collision =
                kek_kekro_check_collision(&kekro, &kekris_field);

            bool moved = prev_x != kekro.x;

            if (collision != NONE_COLLISION) {
                kekro.x = prev_x;
                kekro.y = prev_y;
            }

            if (collision == BOTTOM_COLLISION && !moved) {
                kek_kekro_blit(&kekro, &kekris_field);
                kek_kekro_select_random(&kekro);
                kek_field_check_lines(&kekris_field);
                kekro.x = 4;
                kekro.y = 0;
                collision = kek_kekro_check_collision(&kekro, &kekris_field);
                if (collision != NONE_COLLISION) {
                    printf("YOU DIED\n");
                    kek_field_cleanup(&kekris_field);
                }
                continue;
            }
        }

        SDL_GetWindowSizeInPixels(window, &screen_w, &screen_h);
        field_rect.h = screen_h;
        field_rect.w = screen_h / KEKRIS_HEIGHT * KEKRIS_WIDTH;
        field_rect.x = screen_w / 2 - field_rect.w / 2;
        field_rect.y = 0;
        cell_size_px = field_rect.w / KEKRIS_WIDTH;

        SETCLR(150, 150, 150);
        SDL_RenderClear(renderer);
        SETCLR(230, 230, 230);
        SDL_RenderFillRect(renderer, &field_rect);
        SDL_RenderDrawRect(renderer, &field_rect);

        {
            for (int y = 0; y < KEKRIS_HEIGHT; y += 1) {
                for (int x = 0; x < KEKRIS_WIDTH; x += 1) {
                    int idx = y * KEKRIS_WIDTH + x;
                    select_color(renderer, kekris_field.blocks[idx]);
                    SDL_Rect block =
                        (SDL_Rect){.x = cell_size_px * x + field_rect.x,
                                   .y = cell_size_px * y + field_rect.y,
                                   .h = cell_size_px,
                                   .w = cell_size_px};

                    if (kekris_field.blocks[idx] == 0) {
                        SDL_RenderDrawRect(renderer, &block);
                    } else {
                        SDL_RenderFillRect(renderer, &block);
                    }
                }
            }

            for (int y = 0; y < kekro.size; y += 1) {
                for (int x = 0; x < kekro.size; x += 1) {
                    int idx = y * 4 + x;
                    SDL_Rect block = (SDL_Rect){
                        .x = cell_size_px * (x + kekro.x) + field_rect.x,
                        .y = cell_size_px * (y + kekro.y) + field_rect.y,
                        .h = cell_size_px,
                        .w = cell_size_px};
                    if (kekro.blocks[idx] != 0) {
                        select_color(renderer, kekro.blocks[idx]);
                        SDL_RenderFillRect(renderer, &block);
                        SDL_RenderDrawRect(renderer, &block);
                    }
                }
            }
        }

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();

    return 0;
}
