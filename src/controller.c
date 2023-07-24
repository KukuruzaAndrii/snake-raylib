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
		if (c->sn.head->x == eat->x && c->sn.head->y == eat->y) {
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
	struct snake *s = &c->sn;
	struct level l = c->levels[c->curr_level];

	return snake_check_wall_crush(s, l.w_tile_count, l.h_tile_count) ||
		snake_check_self_eat(s);

}





void UpdateFrame(union screen_ctx *ctx) {
	struct main_screen_ctx *c = &ctx->main_sc;
	struct snake *s = &c->sn;
	c->is_game_over = check_game_over(c);
	if (c->is_game_over) {
		return;
	}

	snake_grow(s);

	c->is_eat = checkEat(c);
	if (!c->is_eat) {
		snake_shrink(s);
	}
	if (c->already_eat_count == c->levels[c->curr_level].eat_count) {
		c->is_open_next_level_portal = 1;
	}
	if (c->is_open_next_level_portal) {
		if (s->head->x == c->levels[c->curr_level].nl_portal.x &&
		    s->head->y == c->levels[c->curr_level].nl_portal.y) {
			c->is_warping_to_next_level = 1;
		}
	}

	if (c->is_warping_to_next_level) {
		snake_remove_head(s);
	}
}
