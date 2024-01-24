#pragma once

#include "common.h"

#define MAX_EAT_COUNT 10

enum eat_state {
	EAT_NO_EAT = 0,
	EAT_LIVE,
};

struct eat {
	int x;
	int y;
};

struct next_level_portal {
	int x;
	int y;
};

/* change while game */
struct level_state {
	int already_eat_count;
	unsigned is_open_next_level_portal: 1;
	unsigned is_warping_to_next_level: 1;
	enum eat_state eat_states[MAX_EAT_COUNT];
};

/* do not change while game */
struct level_def {
	int w_tile_count;
	int h_tile_count;
	struct Vector2 snake_start;
	enum dir snake_start_dir;
	struct next_level_portal nl_portal;
	int eat_count;
	struct eat eats[MAX_EAT_COUNT];
};

struct level {
	struct level_state* st;
	struct level_def* def;
};

struct level_def* get_level_defs(void);

struct level_state* get_level_states(void);
