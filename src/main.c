/*
 *   Snake Hike Game
 *   It's game about snake mechanic in different views
 *
 *   This game has been created using raylib (www.raylib.com)
 *   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
 *
 */

#include <stddef.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "raylib.h"

#include "game.h"
#include "controller.h"
#include "draw.h"

#define countof(_a) ((sizeof _a)/(sizeof _a[0]))

// GRID_SIZE
#define GRID_PX 45
#define EAT_PX_SZ (GRID_PX/2)

#define SCREEN_WIDTH  1920
#define SCREEN_HEIGHT 1080

#define W_TILE_COUNT (SCREEN_WIDTH/GRID_PX)
#define H_TILE_COUNT (SCREEN_HEIGHT/GRID_PX)

#define W_TILE_LAST_NUM (W_TILE_COUNT - 1)
#define H_TILE_LAST_NUM (H_TILE_COUNT - 1)

#define GRID_W_LINE_COUNT (W_TILE_COUNT + 1)
#define GRID_H_LINE_COUNT (H_TILE_COUNT + 1)

#define SCREEN_W_OFFSET ((SCREEN_WIDTH - W_TILE_COUNT * GRID_PX)/2)
#define SCREEN_H_OFFSET ((SCREEN_HEIGHT - H_TILE_COUNT * GRID_PX)/2)

#define X(_x) (_x + SCREEN_W_OFFSET)
#define Y(_y) (_y + SCREEN_H_OFFSET)

#define LABEL_GAME_OVER_SIZE 190

Font font = { 0 };
Music music = { 0 };
Sound fxCoin = { 0 };

static int framesCounter = 0;
static int tickInFrames = 7;

#define IS_DIR_HOR(_dir) ((_dir) == DIR_LEFT || (_dir) == DIR_RIGHT)
#define IS_DIR_VER(_dir) ((_dir) == DIR_UP || (_dir) == DIR_DOWN)

static void init() {
	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Snake hike game");
	ToggleFullscreen();
	InitAudioDevice();	    // Initialize audio device

	// Load global data (assets that must be available in all screens, i.e. font)
	// font = LoadFontEx("resources/TiltNeon-Regular.ttf", LABEL_GAME_OVER_SIZE + 200, NULL, 0);
	font = LoadFontEx("resources/m6x11.ttf", LABEL_GAME_OVER_SIZE + 200, NULL, 0);
	music = LoadMusicStream("resources/ambient.ogg");
	fxCoin = LoadSound("resources/coin.wav");

	SetMusicVolume(music, 1.0f);
	PlayMusicStream(music);
}

static void init_levels(struct game_ctx* g) {
	struct level levels[3] = {
		{
			.w_tile_count = 15, .h_tile_count = 10, .eat_count = 4,
			.eats = {
				{2, 2, EAT_LIVE},
				{3, 3, EAT_LIVE},
				{4, 4, EAT_LIVE},
				{5, 5, EAT_LIVE},
			},
			.nl_portal = {14, 9}
		},
		{.w_tile_count = 5, .h_tile_count = 7, .eat_count = 5},
		{.w_tile_count = 5, .h_tile_count = 7, .eat_count = 5},
	};
	memcpy(g->levels, levels, sizeof(g->levels));
}

static struct node * addNode(struct node *head, int x, int y) {
	struct node *n = (struct node *)malloc(sizeof(struct node));
	n->x = x;
	n->y = y;
	n->next = NULL;
	head->next = n;
	return n;
}

static void set_init_values(struct game_ctx* g) {
	// free previous memory in case of restart
	for (struct node *n = g->head->next, *temp; n != NULL;) {
		temp = n->next;
		TraceLog(LOG_WARNING, "%p", n);
		free(n);
		n = temp;
	}

	g->head->x = 2;
	g->head->y = 0;
	g->head->next = NULL;

	addNode(addNode(g->head, 1, 0), 0, 0);

	g->dir = DIR_RIGHT;

	g->score = 0;
	g->is_ticked = 0;
	g->is_game_over = 0;
	g->is_was_pressed_before_tick = 0;
	g->is_start_screen = 1;
	g->is_exit = 0;
	g->start_screen_snake_x = -4;
	g->menu_count = 2;
	g->selected_menu = 0;

	init_levels(g);
	g->curr_level = 0;
	g->already_eat_count = 0;
	g->is_open_next_level_portal = 0;
	g->is_warping_to_next_level = 0;
	g->level_start_x = 0;
	g->level_start_y = 0;
}

static struct game_ctx* init_game(void) {
	struct game_ctx *g = (struct game_ctx *)malloc(sizeof(struct game_ctx));

	struct node *n = (struct node *)malloc(sizeof(struct node));
	g->head = n;
	g->head->next = NULL;
	set_init_values(g);
	return g;
}
static void deinit(struct game_ctx* g);
void handleControl(struct game_ctx *g) {
	if (g->is_start_screen) {
		if (IsKeyPressed(KEY_DOWN)) {
			if (g->selected_menu == g->menu_count - 1) {
				g->selected_menu = 0;
			} else {
				g->selected_menu++;
			}
		} else if (IsKeyPressed(KEY_UP)) {
			if (g->selected_menu == 0) {
				g->selected_menu = g->menu_count - 1;
			} else {
				g->selected_menu--;
			}
		} else if (IsKeyPressed(KEY_ENTER)) {
			if (g->selected_menu == 0) {
				g->is_start_screen = 0;
			} else if (g->selected_menu == 1) {
				g->is_exit = 1;
			}
		}
		return;
	}

	if (g->is_game_over) {
		if (IsKeyPressed(KEY_R)) {
			int curr_level = g->curr_level;
			set_init_values(g);
			g->is_start_screen = 0;
			g->curr_level = curr_level;
		}
		return;
	}

	if (g->is_was_pressed_before_tick) {
		return;
	}


	if (g->is_warping_to_next_level) {
		return;
	}

	if (IsKeyPressed(KEY_UP)) {
		if (IS_DIR_HOR(g->dir)) {
			g->dir = DIR_UP;
			g->is_was_pressed_before_tick = true;
		}
	} else if (IsKeyPressed(KEY_RIGHT)) {
		if (IS_DIR_VER(g->dir)) {
			g->dir = DIR_RIGHT;
			g->is_was_pressed_before_tick = true;
		}
	} else if (IsKeyPressed(KEY_DOWN)) {
		if (IS_DIR_HOR(g->dir)) {
			g->dir = DIR_DOWN;
			g->is_was_pressed_before_tick = true;
		}
	} else if (IsKeyPressed(KEY_LEFT)) {
		if (IS_DIR_VER(g->dir)) {
			g->dir = DIR_LEFT;
			g->is_was_pressed_before_tick = true;
		}
	}
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
	set_init_values(g);
	free(g->head);
	free(g);

	// Unload global data loaded
	UnloadFont(font);
	UnloadMusicStream(music);
	UnloadSound(fxCoin);

	CloseAudioDevice();	    // Close audio context

	CloseWindow();	    // Close window and OpenGL context
}

int main(void) {
	init();

	SetTargetFPS(60);


	struct game_ctx *g = init_game();

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

