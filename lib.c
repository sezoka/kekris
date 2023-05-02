#include "lib.h"

void kek_set_block(kek_field_t* field, int x, int y, kek_color_t color) {
    field->blocks[y * KEKRIS_WIDTH + x] = color;
}

void copy_kekro(kek_kekro_t* kekro, int8_t* kekro_data, int size) {
    kekro->size = size;
    kekro->size = size;
    for (int y = 0; y < size; y += 1) {
        for (int x = 0; x < size; x += 1) {
            kekro->blocks[y * 4 + x] = kekro_data[y * size + x];
        }
    }
}

void kek_kekro_select(kek_kekro_name_t name, kek_kekro_t* kekro) {
    switch (name) {
        case KEKRO_T:
            return copy_kekro(kekro, SHAPE_T, 3);
        case KEKRO_I:
            return copy_kekro(kekro, SHAPE_I, 4);
        case KEKRO_O:
            return copy_kekro(kekro, SHAPE_O, 2);
        case KEKRO_J:
            return copy_kekro(kekro, SHAPE_J, 3);
        case KEKRO_L:
            return copy_kekro(kekro, SHAPE_L, 3);
        case KEKRO_S:
            return copy_kekro(kekro, SHAPE_S, 3);
        default:
            return copy_kekro(kekro, SHAPE_Z, 3);
    }
}

void kek_kekro_select_random(kek_kekro_t* kekro) {
    kek_kekro_select(rand() % KEKRO_LEN, kekro);
}

static int8_t buffer[4 * 4];
void kek_kekro_rotate(kek_kekro_t* kekro, bool left) {
    int size = kekro->size;

    for (int y = 0; y < size; y += 1) {
        for (int x = 0; x < size; x += 1) {
            if (left) {
                buffer[x * 4 + y] = kekro->blocks[y * 4 + size - x - 1];
            } else {
                buffer[x * 4 + y] = kekro->blocks[(size - y - 1) * 4 + x];
            }
        }
    }

    for (int y = 0; y < size; y += 1) {
        for (int x = 0; x < size; x += 1) {
            kekro->blocks[y * 4 + x] = buffer[y * 4 + x];
        }
    }
}

void remove_line(kek_field_t* field, int line_start) {
    int i = 0;
    while (KEKRIS_WIDTH < line_start) {
        for (int i = line_start; i < line_start + KEKRIS_WIDTH; i += 1) {
            field->blocks[i] = field->blocks[i - KEKRIS_WIDTH];
        }

        i += 1;

        if (500 < i) {
            return;
        }

        line_start -= KEKRIS_WIDTH;
    }
}

int kek_field_check_lines(kek_field_t* field) {
    int score = 0;
    for (int line_start = 0; line_start < KEKRIS_HEIGHT * KEKRIS_WIDTH;
         line_start += KEKRIS_WIDTH) {
        for (int i = line_start; i < line_start + KEKRIS_WIDTH; i += 1) {
            if (field->blocks[i] == 0) {
                goto next_line;
            }
        }
        score += KEKRIS_WIDTH;
        remove_line(field, line_start);
        line_start -= KEKRIS_WIDTH;

    next_line:
        continue;
    }
    return score;
}

void kek_kekro_rotate_left(kek_kekro_t* kekro) {
    kek_kekro_rotate(kekro, true);
}

void kek_kekro_rotate_right(kek_kekro_t* kekro) {
    kek_kekro_rotate(kekro, false);
}

kek_collision_t kek_kekro_check_collision(kek_kekro_t* kekro,
                                          kek_field_t* field) {
    bool left_collision = false;
    bool right_collision = false;
    for (int x = 0; x < kekro->size; x += 1) {
        for (int y = 0; y < kekro->size; y += 1) {
            if (kekro->blocks[y * 4 + x] != 0) {
                int real_x = kekro->x + x;
                int real_y = kekro->y + y;

                if (real_x < 0) {
                    return LEFT_COLLISION;
                }
                if (KEKRIS_WIDTH <= real_x) {
                    return RIGHT_COLLISION;
                }

                if (KEKRIS_HEIGHT <= real_y) {
                    return BOTTOM_COLLISION;
                }

                if (field->blocks[(real_y)*KEKRIS_WIDTH + (real_x)] != 0) {
                    if (y == kekro->size - 1 ||
                        kekro->blocks[(y + 1) * 4 + x] == 0) {
                        return BOTTOM_COLLISION;
                    }
                    if (y < kekro->size / 2) {
                        left_collision = true;
                    } else {
                        right_collision = true;
                    }
                }
            }
        }
    }
    if (left_collision && right_collision) {
        return LEFTRIGHT_COLLISION;
    }
    if (left_collision) {
        return LEFT_COLLISION;
    }
    if (right_collision) {
        return RIGHT_COLLISION;
    }
    return NONE_COLLISION;
}

void kek_kekro_blit(kek_kekro_t* kekro, kek_field_t* field) {
    for (int y = 0; y < kekro->size; y += 1) {
        for (int x = 0; x < kekro->size; x += 1) {
            int field_idx = (y + kekro->y) * KEKRIS_WIDTH + (x + kekro->x);
            if (field->blocks[field_idx] == 0) {
                field->blocks[field_idx] = kekro->blocks[y * 4 + x];
            }
        }
    }
}

void kek_field_cleanup(kek_field_t* field) {
    for (int i = 0; i < KEKRIS_SIZE; i += 1) {
        field->blocks[i] = 0;
    }
}
