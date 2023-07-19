/*
 *   Snake Hike Game
 *   It's game about snake mechanic in different views
 *
 *   This game has been created using raylib (www.raylib.com)
 *   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
 *
 */


#include <stdlib.h>

#include "raylib.h"

#include "game.h"
#include "init.h"
#include "controller.h"
#include "draw.h"
#include "input.h"
#include "resource.h"
#include "screen.h"
#include "screen_table.h"

Music music = { 0 };
Sound fxCoin = { 0 };

static int framesCounter = 0;
static int tickInFrames = 7;

static void init() {
	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Snake hike game");
	ToggleFullscreen();
	InitAudioDevice();	    // Initialize audio device
	init_resources();
	music = LoadMusicStream("resources/ambient.ogg");
	fxCoin = LoadSound("resources/coin.wav");

	SetMusicVolume(music, 1.0f);
	PlayMusicStream(music);
}

static void tick(struct game_ctx *g) {
	framesCounter += 1;
	if (g->is_ticked) {
		g->is_ticked = false;
		g->is_was_pressed_before_tick = false;
	}
	if (framesCounter >= tickInFrames) {
		framesCounter = 0;
		g->is_ticked = true;
	}
}

static void deinit(struct game_ctx *g) {
	// for free snake
	//set_init_values(g);

	free(g->head);
	free(g);

	// Unload global data loaded
	deinit_resources();
	UnloadMusicStream(music);
	UnloadSound(fxCoin);

	CloseAudioDevice();	    // Close audio context

	CloseWindow();	    // Close window and OpenGL context
}

int main(void) {
	init();

	SetTargetFPS(60);

	struct game_ctx *g = init_game();
	TraceLog(LOG_INFO, "init_game");
	enum screen_type next_sc_t = g->screen->type;
	union screen_ctx* sc = &(union screen_ctx){0};
	g->screen->init(sc);
	// Main game loop
	TraceLog(LOG_INFO, "start main loop");
	while (!WindowShouldClose() && g->screen->type != SCREEN_EXIT) {
		tick(g);
		next_sc_t = g->screen->handle_input(sc);
		//handleControl(g);
		if (next_sc_t != g->screen->type) {
			TraceLog(LOG_INFO, "new screen %d>>%d", g->screen->type, next_sc_t);
			struct screen* new_scr = get_screen(next_sc_t);
			new_scr->init(sc);
			g->screen = new_scr;
		}
		if (g->is_ticked) {
			g->screen->update(sc);
			//UpdateFrame(g);
		}
		g->screen->draw(sc);
		//DrawFrame(g);
	}

	deinit(g);

	return 0;
}

