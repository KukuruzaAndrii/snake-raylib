#pragma once

enum screen_type {
	SCREEN_EXIT = -1,
	SCREEN_START = 0,
	SCREEN_GAME,
	SCREEN_COUNT
};

struct start_screen_ctx {
	int start_screen_snake_x;
	int start_screen_snake_y;
	int st_sc_state_cur;
	int st_sc_cntr;
	int menu_count;
	int selected_menu;
};

union screen_ctx {
	struct start_screen_ctx *sc_ctx;
};

struct screen {
	void (*init)(union screen_ctx *sc);
	enum screen_type (*handle_input)(union screen_ctx *sc);
	void (*update)(union screen_ctx *sc);
	void (*draw)(union screen_ctx *sc);
};

struct screen screens[SCREEN_COUNT] = {0};
