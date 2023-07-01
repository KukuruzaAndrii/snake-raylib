#pragma once

#include <stddef.h>

#define foreach_node(_head, _n) for(struct node *_n = _head; _n != NULL; _n = _n->next)

struct node {
	int x;
	int y;
	struct node *next;
};

enum dir {
	DIR_UP = 0,
	DIR_RIGHT,
	DIR_DOWN,
	DIR_LEFT,
};



enum eat_state {
	EAT_NO_EAT = 0,
	EAT_LIVE,
};

struct eat {
	int x;
	int y;
	enum eat_state st;
};

struct next_level_portal {
	int x;
	int y;
};

struct level {
	int w_tile_count;
	int h_tile_count;
	int eat_count;
	struct eat eats[10];
	struct next_level_portal nl_portal;
};

struct game_ctx {
	struct node * head;
	enum dir dir;
	int score;
	unsigned is_ticked:1;
	unsigned is_game_over:1;
	unsigned is_eat:1;
	unsigned is_was_pressed_before_tick:1;
	unsigned is_start_screen:1;
	unsigned is_exit:1;
	int start_screen_snake_x;
	int menu_count;
	int selected_menu;
	struct level levels[3];
	int curr_level;
	int already_eat_count;
	unsigned is_open_next_level_portal:1;
	unsigned is_warping_to_next_level:1;

	int level_start_x;
	int level_start_y;
};

