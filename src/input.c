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

	ret.is_input_processed = snake_input_proceed(&c->sn);

	return ret;
}

