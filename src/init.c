#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "game.h"
#include "init.h"
#include "levels.h"

static struct node* addNode(struct node* head, int x, int y) {
	struct node* n = (struct node*) malloc(sizeof(struct node));
	n->x = x;
	n->y = y;
	n->next = NULL;
	head->next = n;
	return n;
}

static void init_snake(struct node* head, int len, enum dir dir, struct level_def* l) {
	struct node* node = head;

	if (dir == DIR_RIGHT) {
		/* grow left */
		assert ((head->x >= len - 1) && (head->x < l->w_tile_count));

		while (len > 0) {
			node = addNode(node, node->x - 1, node->y);
			len -= 1;
		}
	} else if (dir == DIR_LEFT) {
		/* grow right */
		assert ((head->x >= 0) && (head->x + len < l->w_tile_count));

		while (len > 0) {
			node = addNode(node, node->x + 1, node->y);
			len -= 1;
		}
	} else if (dir == DIR_DOWN) {
		/* grow up */
		assert ((head->y >= len - 1) && (head->y < l->h_tile_count));

		while (len > 0) {
			node = addNode(node, node->x, node->y - 1);
			len -= 1;
		}
	} else if (dir == DIR_UP) {
		/* grow down */
		assert ((head->y >= 0) && (head->y + len < l->h_tile_count));

		while (len > 0) {
			node = addNode(node, node->x, node->y + 1);
			len -= 1;
		}

	}
}

static void init_level_values(struct level_state* st) {
	for (unsigned i = 0; i < countof(st->eat_states); ++i) {
		st->eat_states[i] = EAT_LIVE;
	}
	st->already_eat_count = 0;
	st->is_open_next_level_portal = 0;
	st->is_warping_to_next_level = 0;
}

static void init_curr_level(struct game_ctx* g, int level_num) {
	g->curr_level_num = level_num;

	g->curr_level.def = &g->level_defs[g->curr_level_num];
	g->curr_level.st = &g->level_states[g->curr_level_num];
	init_level_values(g->curr_level.st);
	g->dir = g->curr_level.def->snake_start_dir;

	g->head->x = g->curr_level.def->snake_start.x;
	g->head->y = g->curr_level.def->snake_start.y;
	g->head->next = NULL;

	init_snake(g->head, 3, g->curr_level.def->snake_start_dir, g->curr_level.def);
}

static void init_game_values(struct game_ctx* g) {
	// free previous memory in case of restart
	for (struct node* n = g->head->next, * temp; n != NULL;) {
		temp = n->next;
		//TraceLog(LOG_WARNING, "%p", n);
		free(n);
		n = temp;
	}

	g->score = 0;
	g->is_ticked = 0;
	g->is_game_over = 0;
	g->is_was_pressed_before_tick = 0;
	g->is_start_screen = 0;
	g->is_exit = 0;
	g->start_screen_snake_x = -4;
	g->menu_count = 2;
	g->selected_menu = 0;

	g->level_start_x = 0;
	g->level_start_y = 0;

	g->level_defs = get_level_defs();
	g->level_states = get_level_states();

	init_curr_level(g, 0);
}


struct game_ctx* init_game(void) {
	struct game_ctx* g = (struct game_ctx*) malloc(sizeof(struct game_ctx));

	struct node* n = (struct node*) malloc(sizeof(struct node));
	g->head = n;
	g->head->next = NULL;
	init_game_values(g);
	return g;
}

void reset_game(struct game_ctx* g) {
	init_game_values(g);
}

void reset_level(struct game_ctx* g) {
	g->is_start_screen = 0;
	g->is_game_over = 0;
	init_curr_level(g, g->curr_level_num);
}
