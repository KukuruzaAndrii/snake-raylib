#include <stdlib.h>
#include <string.h>

#include "game.h"
#include "init.h"

static struct node * addNode(struct node *head, int x, int y) {
	struct node *n = (struct node *)malloc(sizeof(struct node));
	n->x = x;
	n->y = y;
	n->next = NULL;
	head->next = n;
	return n;
}


static void init_levels(struct game_ctx* g) {
	struct level levels[3] = {
		{
			.w_tile_count = 15, .h_tile_count = 10, .eat_count = 4,
			.eats = {
				{2, 2, EAT_LIVE},
				{3, 3, EAT_LIVE},
				{4, 4, EAT_LIVE},
				{5, 5, EAT_LIVE},
			},
			.nl_portal = {14, 9}
		},
		{.w_tile_count = 5, .h_tile_count = 7, .eat_count = 5},
		{.w_tile_count = 5, .h_tile_count = 7, .eat_count = 5},
	};
	memcpy(g->levels, levels, sizeof(g->levels));
}

static void set_init_values(struct game_ctx* g) {
	// free previous memory in case of restart
	for (struct node *n = g->head->next, *temp; n != NULL;) {
		temp = n->next;
		//TraceLog(LOG_WARNING, "%p", n);
		free(n);
		n = temp;
	}

	g->head->x = 2;
	g->head->y = 0;
	g->head->next = NULL;

	addNode(addNode(g->head, 1, 0), 0, 0);

	g->dir = DIR_RIGHT;

	g->score = 0;
	g->is_ticked = 0;
	g->is_game_over = 0;
	g->is_was_pressed_before_tick = 0;
	g->is_exit = 0;

	g->is_start_screen = 1;

	init_levels(g);
	g->curr_level = 0;
	g->already_eat_count = 0;
	g->is_open_next_level_portal = 0;
	g->is_warping_to_next_level = 0;
	g->level_start_x = 0;
	g->level_start_y = 0;
}


struct game_ctx* init_game(void) {
	struct game_ctx *g = (struct game_ctx *)malloc(sizeof(struct game_ctx));

	struct node *n = (struct node *)malloc(sizeof(struct node));
	g->head = n;
	g->head->next = NULL;
	set_init_values(g);
	return g;
}

void reset_game(struct game_ctx* g) {
	set_init_values(g);
}

void reset_level(struct game_ctx* g) {
	int curr_level = g->curr_level;
	set_init_values(g);
	g->is_start_screen = 0;
	g->curr_level = curr_level;
}
 
