#include <stdbool.h>

#include "raylib.h"

#include "game.h"
#include "init.h"

#include "input.h"


void handleControl(struct game_ctx *g) {
	if (g->is_game_over) {
		if (IsKeyPressed(KEY_R)) {
			reset_level(g);
		}
		return;
	}

	if (g->is_was_pressed_before_tick) {
		return;
	}


	if (g->is_warping_to_next_level) {
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

