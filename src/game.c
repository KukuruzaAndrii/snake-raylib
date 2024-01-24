/*
 *   Snake Hike Game
 *   It's game about snake mechanic in different views
 *
 *   This game has been created using raylib (www.raylib.com)
 *   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
 *
 */


#include <stdlib.h>
#include <assert.h>

#include "game.h"
#include "init.h"
#include "controller.h"
#include "draw.h"
#include "input.h"


#define X(_x) (_x + SCREEN_W_OFFSET)
#define Y(_y) (_y + SCREEN_H_OFFSET)

#define LABEL_GAME_OVER_SIZE 190

Font font = {0};
Music music = {0};
Sound fxCoin = {0};

static int framesCounter = 0;
static int tickInFrames = 7;

static void init() {
	/* 0 0 - kinda native resolution */
	InitWindow(0, 0, "Snake hike game");
	ToggleFullscreen();
	InitAudioDevice();        // Initialize audio device

	// Load global data (assets that must be available in all screens, i.e. font)
	// font = LoadFontEx("resources/TiltNeon-Regular.ttf", LABEL_GAME_OVER_SIZE + 200, NULL, 0);
	font = LoadFontEx("resources/m6x11.ttf", LABEL_GAME_OVER_SIZE + 200, NULL, 0);
	music = LoadMusicStream("resources/ambient.ogg");
	fxCoin = LoadSound("resources/coin.wav");

	SetMusicVolume(music, 1.0f);
	PlayMusicStream(music);
}

static void tick(struct game_ctx* g) {
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

static void deinit(struct game_ctx* g) {
	// for free snake
	//init_game_values(g);

	free(g->head);
	free(g);

	// Unload global data loaded
	UnloadFont(font);
	UnloadMusicStream(music);
	UnloadSound(fxCoin);

	CloseAudioDevice();        // Close audio context

	CloseWindow();        // Close window and OpenGL context
}

int main(void) {
	init();

	SetTargetFPS(60);

	struct game_ctx* g = init_game();

	// Main game loop
	while (!WindowShouldClose() && !g->is_exit) {
		//	UpdateMusicStream(music);	    // NOTE: Music keeps playing between screens
		tick(g);
		handleControl(g);
		if (g->is_ticked) {
			UpdateFrame(g);
		}
		DrawFrame(g, &font);
	}

	deinit(g);

	return 0;
}

