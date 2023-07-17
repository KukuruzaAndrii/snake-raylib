#pragma once



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

