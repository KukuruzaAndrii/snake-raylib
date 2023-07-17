#include "game.h"
#include "screen_table.h"

struct screen* screen_table[SCREEN_COUNT] = {0};
extern struct screen start_screen;
void init_screen_table(void) {
	screen_table[SCREEN_START] = &start_screen;
}

struct screen* get_screen(enum screen_type screen_type) {
	return screen_table[screen_type];
}
