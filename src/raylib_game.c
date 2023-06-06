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

// GRID_SIZE
#define GRID_PX 43
#define EAT_PX_SZ (GRID_PX/2)

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

static struct node * addNode(struct node *head, int x, int y);

#define foreach_node(_head, _n) for(struct node *_n = _head; _n != NULL; _n = _n->next)

struct node {
	int x;
	int y;
	struct node *next;
};

static int framesCounter = 0;
static int tickInFrames = 8;
static void tick();
static bool isTicked = false;

enum dir {
	DIR_UP = 0,
	DIR_RIGHT,
	DIR_DOWN,
	DIR_LEFT,
};

#define IS_DIR_HOR(_dir) ((_dir) == DIR_LEFT || (_dir) == DIR_RIGHT)
#define IS_DIR_VER(_dir) ((_dir) == DIR_UP || (_dir) == DIR_DOWN)

struct eat {
	int x;
	int y;
};

struct game_ctx {
	struct node * head;
	enum dir dir;
	struct eat eat;
};

struct game_ctx * G = NULL;




static void init() {
	InitWindow(screenWidth, screenHeight, "raylib game template");
//	ToggleFullscreen();
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



struct game_ctx* init_game(void) {
	struct game_ctx *ctx = (struct game_ctx *)malloc(sizeof(struct game_ctx)) ;

	struct node *n = (struct node *)malloc(sizeof(struct node));
	n->x = 10;
	n->y = 10;
	n->next = NULL;

	addNode(addNode(n, 9, 10), 8, 10);

	ctx->head = n;
	ctx->dir = DIR_RIGHT;

	ctx->eat = (struct eat){.x = 20, .y = 15};

	return ctx;
}

void handleControl(struct game_ctx *ctx) {
	if (IsKeyPressed(KEY_UP)) {
		if (IS_DIR_HOR(ctx->dir)) {
			ctx->dir = DIR_UP;
		}
	} else if (IsKeyPressed(KEY_RIGHT)) {
		if (IS_DIR_VER(ctx->dir)) {
			ctx->dir = DIR_RIGHT;
		}
	} else if (IsKeyPressed(KEY_DOWN)) {
		if (IS_DIR_HOR(ctx->dir)) {
			ctx->dir = DIR_DOWN;
		}
	} else if (IsKeyPressed(KEY_LEFT)) {
		if (IS_DIR_VER(ctx->dir)) {
			ctx->dir = DIR_LEFT;
		}
	}
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

static struct node * addNode(struct node *head, int x, int y) {
	struct node *n = (struct node *)malloc(sizeof(struct node));
	n->x = x;
	n->y = y;
	n->next = NULL;
	head->next = n;
	return n;
}

static struct node* growSnake(struct game_ctx *g) {
	struct node *n = (struct node *)malloc(sizeof(struct node));
	switch (g->dir) {
	case DIR_UP:
		n->x = g->head->x;
		n->y = g->head->y - 1;
		break;
	case DIR_RIGHT:
		n->x = g->head->x + 1;
		n->y = g->head->y;
		break;
	case DIR_DOWN:
		n->x = g->head->x;
		n->y = g->head->y + 1;
		break;
	case DIR_LEFT:
		n->x = g->head->x - 1;
		n->y = g->head->y;
		break;
	}
	n->next = g->head;
	return n;
}

static void shrinkSnake(struct game_ctx *g) {
	foreach_node(g->head, n) {
		if (n->next && n->next->next == NULL) {
			free(n->next);
			n->next = NULL;
			break;
		}
	}
}

static void updateSnake(struct game_ctx *g) {

}

static unsigned checkEat(struct game_ctx *g) {
	if (g->head->x == g->eat.x && g->head->y == g->eat.y) {
		g->eat.x = GetRandomValue(0, GRID_W_COUNT);
		g->eat.y = GetRandomValue(0, GRID_H_COUNT);
		return 1;
	}
	return 0;
}

static void updateEat(struct game_ctx *g) {

}


static void UpdateFrame(struct game_ctx *g) {
	if (isTicked) {
		g->head = growSnake(g);

		unsigned is_eat = checkEat(g);
		if (!is_eat) {
			shrinkSnake(g);
		}
	}
}

static void drawGrid(void) {
	for (int i = 0; i < GRID_W_COUNT; i++) {
		DrawLine(i * GRID_PX, 0, i * GRID_PX, screenHeight, LIGHTGRAY);
	}
	for (int i = 0; i < GRID_H_COUNT; i++) {
		DrawLine(0, i * GRID_PX, screenWidth, i * GRID_PX, LIGHTGRAY);
	}
}

static void drawSnake(struct game_ctx *g) {
	foreach_node(g->head, n) {
		// TraceLog(LOG_WARNING, "x=%d y=%d", n->x, n->y);
		DrawRectangle(n->x * GRID_PX, n->y * GRID_PX, GRID_PX, GRID_PX, SKYBLUE);
	}
}

static void drawEat(struct game_ctx *g) {
	DrawRectangle(g->eat.x * GRID_PX + EAT_PX_SZ/2, g->eat.y * GRID_PX + EAT_PX_SZ/2, EAT_PX_SZ, EAT_PX_SZ, MAGENTA);
}


static void DrawFrame(struct game_ctx *g) {
	BeginDrawing();

	ClearBackground(RAYWHITE);
	//DrawLogoScreen();
	//DrawFPS(10, 10);
	drawGrid();
	drawSnake(g);
	drawEat(g);

	EndDrawing();
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

int main(void) {
	init();

	SetTargetFPS(60);
	// TraceLog(LOG_WARNING, "%d %d", GRID_W_COUNT, GRID_H_COUNT);

	G = init_game();

	// Main game loop
	while (!WindowShouldClose()) {
		//	UpdateMusicStream(music);	    // NOTE: Music keeps playing between screens
		tick();
		handleControl(G);
		UpdateFrame(G);
		DrawFrame(G);
	}

	deinit();

	return 0;
}

