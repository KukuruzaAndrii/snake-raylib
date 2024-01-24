#include "game.h"

#include "levels.h"

#define MAX_LEVEL_COUNT 3

static struct level_def level_defs[MAX_LEVEL_COUNT] = {
		{
				.w_tile_count = 15,
				.h_tile_count = 15,
				.nl_portal = {14, 9},
				.snake_start = {
						.x = 6,
						.y = 11,
				},
				.snake_start_dir = DIR_UP,
				.eat_count = 4,
				.eats = {
						{2, 2},
						{3, 3},
						{4, 4},
						{5, 5},
				},
		},
//        {.w_tile_count = 5, .h_tile_count = 7, .eat_count = 5},
//        {.w_tile_count = 5, .h_tile_count = 7, .eat_count = 5},
};

static struct level_state level_states[MAX_LEVEL_COUNT] = {0};

struct level_def* get_level_defs(void) {
	return level_defs;
}

struct level_state* get_level_states(void) {
	return level_states;
}
