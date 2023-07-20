#include <stdlib.h>
#include <string.h>

#include "game.h"
#include "screen.h"
#include "init.h"
#include "screen_table.h"

static struct node * addNode(struct node *head, int x, int y) {
	struct node *n = (struct node *)malloc(sizeof(struct node));
	n->x = x;
	n->y = y;
	n->next = NULL;
	head->next = n;
	return n;
}


static void init_levels(struct main_screen_ctx *c) {
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
	memcpy(c->levels, levels, sizeof(c->levels));
}

static void set_init_values(struct game_ctx* g) {
	g->is_ticked = 0;
	g->is_was_pressed_before_tick = 0;
	init_screen_table();
	g->screen = get_screen(SCREEN_START);
}


struct game_ctx* init_game(void) {
	struct game_ctx *g = (struct game_ctx *)malloc(sizeof(struct game_ctx));

	set_init_values(g);
	return g;
}
/*
void reset_game(struct game_ctx* g) {
	set_init_values(g);
}

void reset_level(struct game_ctx* g) {
	int curr_level = g->curr_level;
	set_init_values(g);
	g->curr_level = curr_level;
}
*/
void init_main_screen (union screen_ctx *ctx) {
	struct main_screen_ctx *c = &(*ctx).main_sc;
	struct node *n = (struct node *)malloc(sizeof(struct node));
	c->head = n;
	c->head->next = NULL;

		// free previous memory in case of restart
	for (struct node *n = g->head->next, *temp; n != NULL;) {
		temp = n->next;
		//TraceLog(LOG_WARNING, "%p", n);
		free(n);
		n = temp;
	}

	c->head->x = 2;
	c->head->y = 0;
	c->head->next = NULL;

	addNode(addNode(c->head, 1, 0), 0, 0);

	c->dir = DIR_RIGHT;

	c->score = 0;
	c->is_ticked = 0;
	c->is_game_over = 0;
	c->is_was_pressed_before_tick = 0;
	c->screen = get_screen(SCREEN_START);

	init_levels(c);
	c->curr_level = 0;
	c->already_eat_count = 0;
	c->is_open_next_level_portal = 0;
	c->is_warping_to_next_level = 0;
	c->level_start_x = 0;
	c->level_start_y = 0;

}
