#include <stdlib.h>

#include "raylib.h"

#include "game.h"
#include "screen.h"

static unsigned checkEat(struct main_screen_ctx *c) {
	struct level* l = &(c->levels[c->curr_level]);
	for (int i = 0; i < l->eat_count; i++) {
		struct eat *eat = &(l->eats[i]);
		if (eat->st != EAT_LIVE) {
			continue;
		}
		if (c->head->x == eat->x && c->head->y == eat->y) {
			//struct Vector2 next_eat_pos = get_next_eat();
			//while (isContainsNodeByVal(c->head, next_eat_pos.x, next_eat_pos.y)) {
			//	next_eat_pos = get_next_eat();
			//}
			//c->eat.x = next_eat_pos.x;
			//c->eat.y = next_eat_pos.y;
			eat->st = EAT_NO_EAT;
			c->score += 1;
			c->already_eat_count += 1;
			return 1;
		}
	}
	return 0;
}

static unsigned check_game_over(struct main_screen_ctx *c) {
	struct Vector2 next_head_pos = get_next_head_pos(c->head, c->dir);
	struct level l = c->levels[c->curr_level];

	// level borders
	if (next_head_pos.x == l.w_tile_count || next_head_pos.y == l.h_tile_count || next_head_pos.x < 0 || next_head_pos.y < 0) {
		return 1;
	}
	// self-eat
	if (isContainsNodeByVal(c->head, next_head_pos.x, next_head_pos.y)) {
		return 1;
	}

	return 0;
}





void UpdateFrame(union screen_ctx *ctx) {
	struct main_screen_ctx *c = &ctx->main_sc;
	c->is_game_over = check_game_over(c);
	if (c->is_game_over) {
		return;
	}

	c->head = growSnake(c);

	c->is_eat = checkEat(c);
	if (!c->is_eat) {
		shrinkSnake(c);
	}
	if (c->already_eat_count == c->levels[c->curr_level].eat_count) {
		c->is_open_next_level_portal = 1;
	}
	if (c->is_open_next_level_portal) {
		if (c->head->x == c->levels[c->curr_level].nl_portal.x &&
		    c->head->y == c->levels[c->curr_level].nl_portal.y) {
			c->is_warping_to_next_level = 1;
		}
	}

	if (c->is_warping_to_next_level) {
		remove_head(c);
	}
}
