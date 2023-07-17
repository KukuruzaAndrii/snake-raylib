#include <stdlib.h>

#include "raylib.h"

#include "game.h"


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

static struct node* growSnake(struct game_ctx *g) {
	struct node *n = (struct node *)malloc(sizeof(struct node));
	struct Vector2 next_head_pos = get_next_head_pos(g->head, g->dir);
	n->x = next_head_pos.x;
	n->y = next_head_pos.y;
	n->next = g->head;
	return n;
}

static void shrinkSnake(struct game_ctx *g) {
	foreach_node(g->head, n) {
		if (n->next && n->next->next == NULL) {
			free(n->next);
			n->next = NULL;
			break;
		}
	}
}

static void remove_head(struct game_ctx *g) {
	if (g->head->next) {
		g->head = g->head->next;
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

static unsigned checkEat(struct game_ctx *g) {
	struct level* l = &(g->levels[g->curr_level]);
	for (int i = 0; i < l->eat_count; i++) {
		struct eat *eat = &(l->eats[i]);
		if (eat->st != EAT_LIVE) {
			continue;
		}
		if (g->head->x == eat->x && g->head->y == eat->y) {
			//struct Vector2 next_eat_pos = get_next_eat();
			//while (isContainsNodeByVal(g->head, next_eat_pos.x, next_eat_pos.y)) {
			//	next_eat_pos = get_next_eat();
			//}
			//g->eat.x = next_eat_pos.x;
			//g->eat.y = next_eat_pos.y;
			eat->st = EAT_NO_EAT;
			g->score += 1;
			g->already_eat_count += 1;
			return 1;
		}
	}
	return 0;
}

static unsigned check_game_over(struct game_ctx *g) {
	struct Vector2 next_head_pos = get_next_head_pos(g->head, g->dir);
	struct level l = g->levels[g->curr_level];

	// level borders
	if (next_head_pos.x == l.w_tile_count || next_head_pos.y == l.h_tile_count || next_head_pos.x < 0 || next_head_pos.y < 0) {
		return 1;
	}
	// self-eat
	if (isContainsNodeByVal(g->head, next_head_pos.x, next_head_pos.y)) {
		return 1;
	}

	return 0;
}





void UpdateFrame(struct game_ctx *g) {
	/* if (g->is_start_screen) { */
	/* 	updateStartScreen(g); */
	/* 	return; */
	/* } */

	g->is_game_over = check_game_over(g);
	if (g->is_game_over) {
		return;
	}

	g->head = growSnake(g);

	g->is_eat = checkEat(g);
	if (!g->is_eat) {
		shrinkSnake(g);
	}
	if (g->already_eat_count == g->levels[g->curr_level].eat_count) {
		g->is_open_next_level_portal = 1;
	}
	if (g->is_open_next_level_portal) {
		if (g->head->x == g->levels[g->curr_level].nl_portal.x &&
		    g->head->y == g->levels[g->curr_level].nl_portal.y) {
			g->is_warping_to_next_level = 1;
		}
	}

	if (g->is_warping_to_next_level) {
		remove_head(g);
	}
}
