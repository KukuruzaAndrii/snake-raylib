// todo remove
#include "init.h"
#include "controller.h"
#include "draw.h"
#include "input.h"
#include "resource.h"
#include "screen.h"

void voidd (union screen_ctx *ctx) {
	(void)(ctx);
}

struct screen game_screen = {
	.init = voidd,
	.handle_input = handleControl,
	.update = UpdateFrame,
	.draw = DrawFrame,
}

