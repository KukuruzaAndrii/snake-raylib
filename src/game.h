#pragma once

#include <stddef.h>

#include "raylib.h"

#include "common.h"
#include "levels.h"

struct game_ctx {
	struct node* head;
	enum dir dir;
	int score;
	unsigned is_ticked: 1;
	unsigned is_game_over: 1;
	unsigned is_eat: 1;
	unsigned is_was_pressed_before_tick: 1;
	unsigned is_start_screen: 1;
	unsigned is_exit: 1;
	int start_screen_snake_x;
	int menu_count;
	int selected_menu;
	struct level_def* level_defs;
	struct level_state* level_states;
	int curr_level_num;
	struct level curr_level;
	int already_eat_count;
	unsigned is_open_next_level_portal: 1;
	unsigned is_warping_to_next_level: 1;

	int level_start_x;
	int level_start_y;
};
