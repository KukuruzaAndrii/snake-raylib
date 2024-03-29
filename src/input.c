#include <stdbool.h>

#include "game.h"
#include "init.h"

#include "input.h"

#define IS_DIR_HOR(_dir) ((_dir) == DIR_LEFT || (_dir) == DIR_RIGHT)
#define IS_DIR_VER(_dir) ((_dir) == DIR_UP || (_dir) == DIR_DOWN)

void handleControl(struct game_ctx* g) {
	if (g->is_start_screen) {
		if (IsKeyPressed(KEY_DOWN)) {
			if (g->selected_menu == g->menu_count - 1) {
				g->selected_menu = 0;
			} else {
				g->selected_menu++;
			}
		} else if (IsKeyPressed(KEY_UP)) {
			if (g->selected_menu == 0) {
				g->selected_menu = g->menu_count - 1;
			} else {
				g->selected_menu--;
			}
		} else if (IsKeyPressed(KEY_ENTER)) {
			if (g->selected_menu == 0) {
				g->is_start_screen = 0;
			} else if (g->selected_menu == 1) {
				g->is_exit = 1;
			}
		}
		return;
	}

	if (IsKeyPressed(KEY_R)) {
		reset_level(g);
		return;
	}

	if (g->is_game_over) {
		return;
	}

	if (g->is_was_pressed_before_tick) {
		return;
	}

	if (g->curr_level.st->is_warping_to_next_level) {
		return;
	}

	if (IsKeyPressed(KEY_UP)) {
		if (IS_DIR_HOR(g->dir)) {
			g->dir = DIR_UP;
			g->is_was_pressed_before_tick = true;
		}
	} else if (IsKeyPressed(KEY_RIGHT)) {
		if (IS_DIR_VER(g->dir)) {
			g->dir = DIR_RIGHT;
			g->is_was_pressed_before_tick = true;
		}
	} else if (IsKeyPressed(KEY_DOWN)) {
		if (IS_DIR_HOR(g->dir)) {
			g->dir = DIR_DOWN;
			g->is_was_pressed_before_tick = true;
		}
	} else if (IsKeyPressed(KEY_LEFT)) {
		if (IS_DIR_VER(g->dir)) {
			g->dir = DIR_LEFT;
			g->is_was_pressed_before_tick = true;
		}
	}
}

