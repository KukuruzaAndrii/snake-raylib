#include "snake.h"

/* init */
static struct node * addNode(struct node *head, int x, int y) {
	struct node *n = (struct node *)malloc(sizeof(struct node));
	n->x = x;
	n->y = y;
	n->next = NULL;
	head->next = n;
	return n;
}
void snake_init(struct snake * s, int head_x, int head_y, int len) {
	assert(len > 0);

	struct node *n = (struct node *)malloc(sizeof(struct node));
	s->head = n;

	s->head->x = head_x;
	s->head->y = head_y;
	s->head->next = NULL;

	for (int i = 0; i < len - 1; i++) {
		n = addNode(n, head_x - i, head_y);
	}

	s->dir = DIR_RIGHT;
}

void snake_set_dir(struct snake * s, enum dir dir) {
	s->dir = dir;
}

/* input */
unsigned snake_input_proceed(struct snake * s) {
	unsigned is_input_processed = 0;

	if (IsKeyPressed(KEY_UP)) {
		if (IS_DIR_HOR(s->dir)) {
			s->dir = DIR_UP;
			is_input_processed = 1;
		}
	} else if (IsKeyPressed(KEY_RIGHT)) {
		if (IS_DIR_VER(s->dir)) {
			s->dir = DIR_RIGHT;
			is_input_processed = 1;
		}
	} else if (IsKeyPressed(KEY_DOWN)) {
		if (IS_DIR_HOR(s->dir)) {
			s->dir = DIR_DOWN;
			is_input_processed = 1;
		}
	} else if (IsKeyPressed(KEY_LEFT)) {
		if (IS_DIR_VER(s->dir)) {
			s->dir = DIR_LEFT;
			is_input_processed = 1;
		}
	}

	return is_input_processed;
}


/* controller */
static bool isContainsNodeByVal(struct node *head, int x, int y) {
	foreach_node(head, n) {
		if (n->x == x && n->y == y) {
			return true;
		}
	}
	return false;
}

Vector2 get_next_head_pos(struct snake* sn) {
	struct Vector2 v = {0};
	switch (sn->dir) {
	case DIR_UP:
		v.x = sn->head->x;
		v.y = sn->head->y - 1;
		break;
	case DIR_RIGHT:
		v.x = sn->head->x + 1;
		v.y = sn->head->y;
		break;
	case DIR_DOWN:
		v.x = sn->head->x;
		v.y = sn->head->y + 1;
		break;
	case DIR_LEFT:
		v.x = sn->head->x - 1;
		v.y = sn->head->y;
		break;
	}
	return v;
}

bool snake_check_self_eat(struct snake *s) {
	struct Vector2 next_head_pos = get_next_head_pos(s);
	if (isContainsNodeByVal(s->head, next_head_pos.x, next_head_pos.y)) {
		return 1;
	}
	return 0;
}

bool snake_check_wall_crush(struct snake *s, int w_tile_count, int h_tile_count) {
	struct Vector2 next_head_pos = get_next_head_pos(s);

	if (next_head_pos.x == w_tile_count || next_head_pos.y == h_tile_count || next_head_pos.x < 0 || next_head_pos.y < 0) {
		return 1;
	}
	return 0;
}

void snake_grow(struct snake *s) {
	struct node *new = (struct node *)malloc(sizeof(struct node));
	struct Vector2 next_head_pos = get_next_head_pos(s);
	new->x = next_head_pos.x;
	new->y = next_head_pos.y;
	new->next = s->head;
	s->head = new;
}

void snake_shrink(struct snake *s) {
	foreach_node(s->head, n) {
		if (n->next && n->next->next == NULL) {
			free(n->next);
			n->next = NULL;
			break;
		}
	}
}

void snake_remove_head(struct snake *s) {
	if (s->head->next) {
		s->head = s->head->next;
	}
}



/* draw */

#define SNAKE_BODY_COLOR SKYBLUE
#define SNAKE_HEAD_COLOR BLUE

#define SNAKE_GAME_OVER_COLOR RED
#define SNAKE_EAT_COLOR MAGENTA

void snake_draw(struct snake* s, int level_x_offset, int level_y_offset) {
	Color head_c = SNAKE_HEAD_COLOR;
	Color body_c = SNAKE_BODY_COLOR;
	/*	if (s->is_eat) {
		head_c = SNAKE_EAT_COLOR;
	}
	if (s->is_game_over) {
		body_c = SNAKE_GAME_OVER_COLOR;
		head_c = SNAKE_GAME_OVER_COLOR;
	}
	if (s->is_warping_to_next_level) {
		head_c = SNAKE_BODY_COLOR;
		}
	*/

	DrawRectangle(s->head->x * GRID_PX + level_x_offset, s->head->y * GRID_PX + level_y_offset, GRID_PX, GRID_PX, head_c);
	foreach_node(s->head->next, n) {
		// TraceLog(LOG_WARNING, "x=%d y=%d", n->x, n->y);
		DrawRectangle(n->x * GRID_PX + level_x_offset, n->y * GRID_PX + level_y_offset, GRID_PX, GRID_PX, body_c);
	}
}
