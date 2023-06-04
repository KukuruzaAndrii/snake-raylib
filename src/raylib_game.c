/*******************************************************************************************
 *
 *   raylib game template
 *
 *   <Game title>
 *   <Game description>
 *
 *   This game has been created using raylib (www.raylib.com)
 *   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
 *
 *   Copyright (c) 2021 Ramon Santamaria (@raysan5)
 *
 ********************************************************************************************/

#include <stddef.h>
#include <stdlib.h>

#include "raylib.h"
#include "screen_logo.h"

#define GRID_PX 20
#define GRID_W_COUNT (screenWidth/GRID_PX + 1)
#define GRID_H_COUNT (screenHeight/GRID_PX + 1)
//----------------------------------------------------------------------------------
// Shared Variables Definition (global)
// NOTE: Those variables are shared between modules through screens.h
//----------------------------------------------------------------------------------
Font font = { 0 };
Music music = { 0 };
Sound fxCoin = { 0 };

//----------------------------------------------------------------------------------
// Local Variables Definition (local to this module)
//----------------------------------------------------------------------------------
static const int screenWidth = 1920;
static const int screenHeight = 1080;

//----------------------------------------------------------------------------------
// Local Functions Declaration
//----------------------------------------------------------------------------------
static void init();
static void deinit();

static void UpdateFrame(void);	    // Update one frame
static void DrawFrame(void);	    // Draw one frame

static struct node * addNode(struct node *head, int x, int y);

#define foreach_node(_n) for(struct node *_n = Sn; _n != NULL; _n = _n->next)

struct node {
	int x;
	int y;
	struct node *next;
};
struct node *Sn = NULL;

static int framesCounter = 0;
static int tickInFrames = 5;
static void tick();
static bool isTicked = false;

enum dir {
	DIR_UP = 0,
	DIR_RIGHT,
	DIR_DOWN,
	DIR_LEFT,
};

enum dir cur_dir = DIR_RIGHT;
//----------------------------------------------------------------------------------
// Main entry point
//----------------------------------------------------------------------------------
int main(void) {
	init();

	SetTargetFPS(60);	    // Set our game to run at 60 frames-per-second
	// TraceLog(LOG_WARNING, "%d %d", GRID_W_COUNT, GRID_H_COUNT);


	struct node head = {.x = 10, .y = 10};
	addNode(addNode(&head, 9, 10), 8, 10);
	Sn = &head;
	// Main game loop
	while (!WindowShouldClose()) {   // Detect window close button or ESC key
		//	UpdateMusicStream(music);	    // NOTE: Music keeps playing between screens
		tick();
		UpdateFrame();
		DrawFrame();
	}

	deinit();

	return 0;
}

static void tick() {
	framesCounter += 1;
	if (isTicked) {
		isTicked = false;
	}
	if (framesCounter >= tickInFrames) {
		framesCounter = 0;
		isTicked = true;
	}
}
static void init() {
	InitWindow(screenWidth, screenHeight, "raylib game template");
	ToggleFullscreen();
	InitAudioDevice();	    // Initialize audio device

	// Load global data (assets that must be available in all screens, i.e. font)
	font = LoadFont("resources/mecha.png");
	music = LoadMusicStream("resources/ambient.ogg");
	fxCoin = LoadSound("resources/coin.wav");

	SetMusicVolume(music, 1.0f);
	PlayMusicStream(music);

	// Setup and init first screen
	InitLogoScreen();
}

static void deinit() {
	// Unload current screen data before closing
	UnloadLogoScreen();

	// Unload global data loaded
	UnloadFont(font);
	UnloadMusicStream(music);
	UnloadSound(fxCoin);

	CloseAudioDevice();	    // Close audio context

	CloseWindow();	    // Close window and OpenGL context
}

static struct node * addNode(struct node *head, int x, int y) {
	struct node *n = (struct node *)malloc(sizeof(struct node));
	n->x = x;
	n->y = y;
	n->next = NULL;
	head->next = n;
	return n;
}

static void updateSnake(void) {
	struct node *n = (struct node *)malloc(sizeof(struct node));
	switch (cur_dir) {
	case DIR_UP:
		n->x = Sn->x;
		n->y = Sn->y - 1;
		break;
	case DIR_RIGHT:
		n->x = Sn->x + 1;
		n->y = Sn->y;
		break;
	case DIR_DOWN:
		n->x = Sn->x;
		n->y = Sn->y + 1;
		break;
	case DIR_LEFT:
		n->x = Sn->x - 1;
		n->y = Sn->y;
		break;
	}
	n->next = Sn;
	Sn = n;
}

// Update game frame
static void UpdateFrame(void) {
	//UpdateLogoScreen();
	if (IsKeyPressed(KEY_UP)) {
		cur_dir = DIR_UP;
	} else if (IsKeyPressed(KEY_RIGHT)) {
		cur_dir = DIR_RIGHT;
	} else if (IsKeyPressed(KEY_DOWN)) {
		cur_dir = DIR_DOWN;
	} else if (IsKeyPressed(KEY_LEFT)) {
		cur_dir = DIR_LEFT;
	}

	if (isTicked) {
		updateSnake();
	}
	
}

static void drawGrid(void) {
	for (unsigned i = 0; i < GRID_W_COUNT; i++) {
		DrawLine(i * GRID_PX, 0, i * GRID_PX, screenHeight, BLACK);
	}
	for (unsigned i = 0; i < GRID_H_COUNT; i++) {
		DrawLine(0, i * GRID_PX, screenWidth, i * GRID_PX, BLACK);
	}
}

static void drawSnake(void) {
	foreach_node(n) {
		// TraceLog(LOG_WARNING, "x=%d y=%d", n->x, n->y);
		DrawRectangle(n->x * GRID_PX, n->y * GRID_PX, GRID_PX, GRID_PX, BLUE);
	}
}

static void DrawFrame(void) {
	BeginDrawing();

	ClearBackground(RAYWHITE);
	//DrawLogoScreen();
	//DrawFPS(10, 10);
	drawGrid();
	drawSnake();

	EndDrawing();
}
