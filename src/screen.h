#pragma once

struct start_screen_ctx {
	int start_screen_snake_x;
	int start_screen_snake_y;
	int st_sc_state_cur;
	int st_sc_cntr;
	int menu_count;
	int selected_menu;
};

struct main_screen_ctx {
	int score;
	unsigned is_game_over:1;
	unsigned is_eat:1;
	
	struct level levels[3];
	int curr_level;
	int already_eat_count;
	unsigned is_open_next_level_portal:1;
	unsigned is_warping_to_next_level:1;
	int level_start_x;
	int level_start_y;
};

union screen_ctx {
	struct start_screen_ctx sc_ctx;
	struct main_screen_ctx main_sc;
};

struct input_ret_ctx {
	enum screen_type new_scr;
	unsigned is_screen_changed:1;
	unsigned is_input_processed:1;
};

struct screen {
	enum screen_type type;
	void (*init)(union screen_ctx *ctx);
	struct input_ret_ctx (*handle_input)(union screen_ctx *ctx);
	void (*update)(union screen_ctx *ctx);
	void (*draw)(union screen_ctx *ctx);
};

