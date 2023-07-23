#include "snake.h"

/* controller */
static bool isContainsNodeByVal(struct node *head, int x, int y) {
	foreach_node(head, n) {
		if (n->x == x && n->y == y) {
			return true;
		}
	}
	return false;
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

static struct node* growSnake(struct snake *s) {
	struct node *n = (struct node *)malloc(sizeof(struct node));
	struct Vector2 next_head_pos = get_next_head_pos(s->head, s->dir);
	n->x = next_head_pos.x;
	n->y = next_head_pos.y;
	n->next = s->head;
	return n;
}

static void shrinkSnake(struct snake *s) {
	foreach_node(s->head, n) {
		if (n->next && n->next->next == NULL) {
			free(n->next);
			n->next = NULL;
			break;
		}
	}
}

static void remove_head(struct snake *s) {
	if (s->head->next) {
		s->head = s->head->next;
	}
}
