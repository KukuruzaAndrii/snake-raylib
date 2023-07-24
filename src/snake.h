#pragma once
#include "game.h"

enum dir {
	DIR_UP = 0,
	DIR_RIGHT,
	DIR_DOWN,
	DIR_LEFT,
};

struct snake {
	struct node * head;
	enum dir dir;
};

void snake_init(struct snake * s, int head_x, int head_y, int len);
void snake_set_dir(struct snake * s, enum dir);

Vector2 get_next_head_pos(struct snake* s);
bool snake_check_self_eat(struct snake *s);
bool snake_check_wall_crush(struct snake *s, int w_tile_count, int h_tile_count);
void snake_grow(struct snake *s);
void snake_shrink(struct snake *s);
void snake_remove_head(struct snake *s);

void snake_draw(struct snake* s, int level_x_offset, int level_y_offset);
unsigned snake_input_proceed(struct snake * s);
