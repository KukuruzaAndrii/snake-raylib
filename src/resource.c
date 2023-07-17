#include "game.h"
#include "resource.h"

Font font = { 0 };

void init_resources(void) {
	font = LoadFontEx("resources/m6x11.ttf", LABEL_GAME_OVER_SIZE + 200, NULL, 0);
}

void deinit_resources(void) {
	UnloadFont(font);
}