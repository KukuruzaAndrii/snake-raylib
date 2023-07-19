#include "game.h"
#include "screen_table.h"

// todo remove
#include "init.h"
#include "controller.h"
#include "draw.h"
#include "input.h"
#include "resource.h"
#include "screen.h"

struct screen* screen_table[SCREEN_COUNT] = {0};
extern struct screen start_screen;
void voidd (union screen_ctx *ctx) {
	(void)(ctx);
}
struct screen game_screen = {
	.init = voidd,
	.handle_input = handleControl,
	.update = UpdateFrame,
	.draw = DrawFrame,
}

void init_screen_table(void) {
	screen_table[SCREEN_START] = &start_screen;
	screen_table[SCREEN_GAME] = &game_screen;
}

struct screen* get_screen(enum screen_type screen_type) {
	return screen_table[screen_type];
}
