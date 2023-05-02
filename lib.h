#pragma once

#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdint.h>

typedef enum {
    KEKRIS_WIDTH = 10,
    KEKRIS_HEIGHT = 20,
    KEKRIS_SIZE = KEKRIS_WIDTH * KEKRIS_HEIGHT
} kekris_dimension_t;
typedef enum { L = 1, B, Y, O, G, P, R } kek_color_t;
typedef enum {
    NONE_COLLISION = 0,
    LEFT_COLLISION,
    RIGHT_COLLISION,
    LEFTRIGHT_COLLISION,
    BOTTOM_COLLISION,
} kek_collision_t;
typedef enum {
    KEKRO_I,
    KEKRO_O,
    KEKRO_T,
    KEKRO_J,
    KEKRO_L,
    KEKRO_S,
    KEKRO_Z,
    KEKRO_LEN,
} kek_kekro_name_t;

typedef struct {
    int8_t blocks[KEKRIS_SIZE];
} kek_field_t;

typedef struct {
    kek_color_t blocks[4 * 4];
    int size;
    int x;
    int y;
} kek_kekro_t;

void kek_set_block(kek_field_t* field, int x, int y, kek_color_t color);

void kek_draw_shape(kek_field_t* field,
                    kek_color_t* shape,
                    int x,
                    int y,
                    int w,
                    int h);

void kek_kekro_select(kek_kekro_name_t name, kek_kekro_t* kekro);

void kek_kekro_select_random(kek_kekro_t* kekro);

void kek_kekro_rotate(kek_kekro_t* kekro, bool left);

void kek_kekro_rotate_left(kek_kekro_t* kekro);

void kek_kekro_rotate_right(kek_kekro_t* kekro);

void kek_field_cleanup(kek_field_t* field);

kek_collision_t kek_kekro_check_collision(kek_kekro_t* kekro,
                                          kek_field_t* field);

void kek_kekro_blit(kek_kekro_t* kekro, kek_field_t* field);

int kek_field_check_lines(kek_field_t* field);

static int8_t SHAPE_I[] = {
    0, 0, 0, 0, L, L, L, L,  //
    0, 0, 0, 0, 0, 0, 0, 0,

};

static int8_t SHAPE_O[] = {Y, Y,  //
                           Y, Y};

static int8_t SHAPE_J[] = {
    0, B, 0,  //
    0, B, 0,  //
    B, B, 0,
};

static int8_t SHAPE_L[] = {
    O, 0, 0,  //
    O, 0, 0,  //
    O, O, 0,  //
};

static int8_t SHAPE_S[] = {0, G, G,  //
                           G, G, 0,  //
                           0, 0, 0};

static int8_t SHAPE_Z[] = {R, R, 0,  //
                           0, R, R,  //
                           0, 0, 0};

static int8_t SHAPE_T[] = {
    0, 0, 0,  //
    P, P, P,  //
    0, P, 0,  //
};
