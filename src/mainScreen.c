// todo remove
#include "init.h"
#include "controller.h"
#include "draw.h"
#include "input.h"
#include "resource.h"
#include "screen.h"

struct screen game_screen = {
	.type = SCREEN_GAME,
	.init = init_main_screen,
	.handle_input = handleControl,
	.update = UpdateFrame,
	.draw = DrawFrame,
};

