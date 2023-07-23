#include <stdbool.h>

#include "raylib.h"

#include "game.h"
#include "init.h"

#include "screen.h"
#include "input.h"


struct input_ret_ctx handleControl(union screen_ctx *ctx) {
	struct main_screen_ctx *c = &ctx->main_sc;
	struct input_ret_ctx ret = {0};
	if (c->is_game_over) {
		if (IsKeyPressed(KEY_R)) {
			//reset_level(g);
		}
		return ret;
	}

	if (c->is_warping_to_next_level) {
		// !!!!
		return ret;
	}

	if (IsKeyPressed(KEY_UP)) {
		if (IS_DIR_HOR(c->dir)) {
			c->dir = DIR_UP;
			ret.is_input_processed = 1;
		}
	} else if (IsKeyPressed(KEY_RIGHT)) {
		if (IS_DIR_VER(c->dir)) {
			c->dir = DIR_RIGHT;
			ret.is_input_processed = 1;
		}
	} else if (IsKeyPressed(KEY_DOWN)) {
		if (IS_DIR_HOR(c->dir)) {
			c->dir = DIR_DOWN;
			ret.is_input_processed = 1;
		}
	} else if (IsKeyPressed(KEY_LEFT)) {
		if (IS_DIR_VER(c->dir)) {
			c->dir = DIR_LEFT;
			ret.is_input_processed = 1;
		}
	}

	return ret;
}

