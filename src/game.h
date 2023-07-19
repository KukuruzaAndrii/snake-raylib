#pragma once

#include <stddef.h>
#include <assert.h>

#include "raylib.h"

#define SCREEN_WIDTH  1920
#define SCREEN_HEIGHT 1080

#define LABEL_GAME_OVER_SIZE 190

#define countof(_a) ((sizeof _a)/(sizeof _a[0]))

#define foreach_node(_head, _n) for(struct node *_n = _head; _n != NULL; _n = _n->next)

struct node {
	int x;
	int y;
	struct node *next;
};

#define IS_DIR_HOR(_dir) ((_dir) == DIR_LEFT || (_dir) == DIR_RIGHT)
#define IS_DIR_VER(_dir) ((_dir) == DIR_UP || (_dir) == DIR_DOWN)

enum dir {
	DIR_UP = 0,
	DIR_RIGHT,
	DIR_DOWN,
	DIR_LEFT,
};

enum eat_state {
	EAT_NO_EAT = 0,
	EAT_LIVE,
};

struct eat {
	int x;
	int y;
	enum eat_state st;
};

struct next_level_portal {
	int x;
	int y;
};

struct level {
	int w_tile_count;
	int h_tile_count;
	int eat_count;
	struct eat eats[10];
	struct next_level_portal nl_portal;
};

//move to screen.h
enum screen_type {
	SCREEN_EXIT = -1,
	SCREEN_START = 0,
	SCREEN_GAME,
	SCREEN_COUNT
};

struct game_ctx {
	struct screen* screen;
	unsigned is_ticked:1;
};

