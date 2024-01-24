#pragma once

#define foreach_node(_head, _n) for(struct node *_n = _head; _n != NULL; _n = _n->next)
#define countof(_a) ((sizeof _a)/(sizeof _a[0]))

struct node {
	int x;
	int y;
	struct node* next;
};

enum dir {
	DIR_UP = 0,
	DIR_RIGHT,
	DIR_DOWN,
	DIR_LEFT,
};
