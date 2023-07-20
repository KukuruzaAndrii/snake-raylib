#include <stdlib.h>

#include "raylib.h"

#include "game.h"
#include "screen.h"


static unsigned isContainsNodeByVal(struct node *head, int x, int y) {
	foreach_node(head, n) {
		if (n->x == x && n->y == y) {
			return 1;
		}
	}
	return 0;
}

static Vector2 get_next_head_pos(struct node* head, enum dir dir) {
	struct Vector2 v = {0};
	switch (dir) {
	case DIR_UP:
		v.x = head->x;
		v.y = head->y - 1;
		break;
	case DIR_RIGHT:
		v.x = head->x + 1;
		v.y = head->y;
		break;
	case DIR_DOWN:
		v.x = head->x;
		v.y = head->y + 1;
		break;
	case DIR_LEFT:
		v.x = head->x - 1;
		v.y = head->y;
		break;
	}
	return v;
}

static struct node* growSnake(struct main_screen_ctx *c) {
	struct node *n = (struct node *)malloc(sizeof(struct node));
	struct Vector2 next_head_pos = get_next_head_pos(c->head, c->dir);
	n->x = next_head_pos.x;
	n->y = next_head_pos.y;
	n->next = c->head;
	return n;
}

static void shrinkSnake(struct main_screen_ctx *c) {
	foreach_node(c->head, n) {
		if (n->next && n->next->next == NULL) {
			free(n->next);
			n->next = NULL;
			break;
		}
	}
}

static void remove_head(struct main_screen_ctx *c) {
	if (c->head->next) {
		c->head = c->head->next;
	}
}

/*
static void updateSnake(struct game_ctx *g) {
}

static Vector2 get_next_eat() {
	return (struct Vector2){
		.x = GetRandomValue(0, W_TILE_LAST_NUM),
		.y = GetRandomValue(0, H_TILE_LAST_NUM),
	};
}
*/

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
	struct main_screen_ctx *c = &(*ctx).main_sc;
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
