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

#define EAT_COLOR MAGENTA

#define SNAKE_BODY_COLOR SKYBLUE
#define SNAKE_HEAD_COLOR BLUE

#define SNAKE_GAME_OVER_COLOR RED
#define SNAKE_EAT_COLOR MAGENTA


Font font = { 0 };
Music music = { 0 };
Sound fxCoin = { 0 };


static struct node * addNode(struct node *head, int x, int y);

#define foreach_node(_head, _n) for(struct node *_n = _head; _n != NULL; _n = _n->next)

struct node {
	int x;
	int y;
	struct node *next;
};

static int framesCounter = 0;
static int tickInFrames = 7;

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
	int score;
	unsigned is_ticked:1;
	unsigned is_game_over:1;
	unsigned is_eat:1;
	unsigned is_was_pressed_before_tick:1;
};

static void init() {
	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "raylib game template");
	ToggleFullscreen();
	InitAudioDevice();	    // Initialize audio device

	// Load global data (assets that must be available in all screens, i.e. font)
	font = LoadFont("resources/mecha.png");
	music = LoadMusicStream("resources/ambient.ogg");
	fxCoin = LoadSound("resources/coin.wav");

	SetMusicVolume(music, 1.0f);
	PlayMusicStream(music);
}


static void set_init_values(struct game_ctx* g) {
	// free previous memory in case of restart
	for(struct node *n = g->head->next, *temp; n != NULL;) {
		temp = n->next;
		TraceLog(LOG_WARNING, "%p", n);
		free(n);
		n = temp;
	}

	g->head->x = 10;
	g->head->y = 10;
	g->head->next = NULL;

	addNode(addNode(g->head, 9, 10), 8, 10);

	g->dir = DIR_RIGHT;

	g->eat = (struct eat){.x = 20, .y = 15};
	g->score = 0;
	g->is_ticked = 0;
	g->is_game_over = 0;
	g->is_was_pressed_before_tick = 0;
}

static struct game_ctx* init_game(void) {
	struct game_ctx *g = (struct game_ctx *)malloc(sizeof(struct game_ctx));

	struct node *n = (struct node *)malloc(sizeof(struct node));
	g->head = n;
	g->head->next = NULL;
	set_init_values(g);
	return g;
}

void handleControl(struct game_ctx *g) {
	if (g->is_game_over) {
		if (IsKeyPressed(KEY_R)) {
			set_init_values(g);
		}
		return;
	}

	if (g->is_was_pressed_before_tick) {
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

static struct node * addNode(struct node *head, int x, int y) {
	struct node *n = (struct node *)malloc(sizeof(struct node));
	n->x = x;
	n->y = y;
	n->next = NULL;
	head->next = n;
	return n;
}

static unsigned isContainsNodeByVal(struct node *head, int x, int y) {
	foreach_node(head, n) {
		if (n->x == x && n->y == y) {
			return 1;
		}
	}
	return 0;
}

static Vector2 get_next_head_pos(struct node* head, enum dir dir) {
	struct Vector2 v = {0};
	switch (dir) {
	case DIR_UP:
		v.x = head->x;
		v.y = head->y - 1;
		break;
	case DIR_RIGHT:
		v.x = head->x + 1;
		v.y = head->y;
		break;
	case DIR_DOWN:
		v.x = head->x;
		v.y = head->y + 1;
		break;
	case DIR_LEFT:
		v.x = head->x - 1;
		v.y = head->y;
		break;
	}
	return v;
}

static struct node* growSnake(struct game_ctx *g) {
	struct node *n = (struct node *)malloc(sizeof(struct node));
	struct Vector2 next_head_pos = get_next_head_pos(g->head, g->dir);
	n->x = next_head_pos.x;
	n->y = next_head_pos.y;
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

static Vector2 get_next_eat() {
	return (struct Vector2){
		.x = GetRandomValue(0, W_TILE_LAST_NUM),
		.y = GetRandomValue(0, H_TILE_LAST_NUM),
	};
}

static unsigned checkEat(struct game_ctx *g) {
	if (g->head->x == g->eat.x && g->head->y == g->eat.y) {
		struct Vector2 next_eat_pos = get_next_eat();
		while (isContainsNodeByVal(g->head, next_eat_pos.x, next_eat_pos.y)) {
			next_eat_pos = get_next_eat();
		}
		g->eat.x = next_eat_pos.x;
		g->eat.y = next_eat_pos.y;
		g->score += 1;
		return 1;
	}
	return 0;
}

static unsigned check_game_over(struct game_ctx *g) {
	struct Vector2 next_head_pos = get_next_head_pos(g->head, g->dir);
	// wall
	if (next_head_pos.x == W_TILE_COUNT || next_head_pos.y == H_TILE_COUNT || next_head_pos.x < 0 || next_head_pos.y < 0) {
		return 1;
	}
	// self-eat
	if (isContainsNodeByVal(g->head, next_head_pos.x, next_head_pos.y)) {
		return 1;
	}

	return 0;
}

static void UpdateFrame(struct game_ctx *g) {
	g->is_game_over = check_game_over(g);
	if (g->is_game_over) {
		return;
	}

	g->head = growSnake(g);

	g->is_eat = checkEat(g);
	if (!g->is_eat) {
		shrinkSnake(g);
	}
}

static void drawGrid(void) {
	for (int i = 0; i < GRID_W_LINE_COUNT; i++) {
		DrawLine(X(i * GRID_PX), Y(0), X(i * GRID_PX), Y(H_TILE_COUNT * GRID_PX), LIGHTGRAY);
	}
	for (int i = 0; i < GRID_H_LINE_COUNT; i++) {
		DrawLine(X(0), Y(i * GRID_PX), X(W_TILE_COUNT * GRID_PX), Y(i * GRID_PX), LIGHTGRAY);
	}
}

static void drawSnake(struct game_ctx *g) {
	Color head_c = SNAKE_HEAD_COLOR;
	Color body_c = SNAKE_BODY_COLOR;
	if (g->is_eat) {
		head_c = SNAKE_EAT_COLOR;
	}
	if (g->is_game_over) {
		body_c = SNAKE_GAME_OVER_COLOR;
		head_c = SNAKE_GAME_OVER_COLOR;
	}

	DrawRectangle(X(g->head->x * GRID_PX), Y(g->head->y * GRID_PX), GRID_PX, GRID_PX, head_c);
	foreach_node(g->head->next, n) {
		// TraceLog(LOG_WARNING, "x=%d y=%d", n->x, n->y);
		DrawRectangle(X(n->x * GRID_PX), Y(n->y * GRID_PX), GRID_PX, GRID_PX, body_c);
	}
}

static void drawEat(struct game_ctx *g) {
	DrawRectangle(X(g->eat.x * GRID_PX + EAT_PX_SZ/2), Y(g->eat.y * GRID_PX + EAT_PX_SZ/2), EAT_PX_SZ, EAT_PX_SZ, EAT_COLOR);
}

static void drawScore(struct game_ctx *g) {
	DrawText("Score:", 40, 40, 40, DARKGRAY);
	DrawText(TextFormat("%d", g->score), 190, 40, 40, RED);
}

static void drawGameOver(struct game_ctx *g) {
	DrawText("GAME OVER", SCREEN_WIDTH/2 - 400, SCREEN_HEIGHT/2 - 70, 140, DARKGRAY);
}

static void DrawFrame(struct game_ctx *g) {
	BeginDrawing();

	ClearBackground(RAYWHITE);
	//DrawLogoScreen();
	//DrawFPS(10, 10);
	drawSnake(g);
	drawGrid();
	drawEat(g);
	drawScore(g);
	if (g->is_game_over) {
		//TraceLog(LOG_WARNING, "is_game_over", GRID_W_COUNT, GRID_H_COUNT);
		drawGameOver(g);
	}

	EndDrawing();
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
	while (!WindowShouldClose()) {
		//	UpdateMusicStream(music);	    // NOTE: Music keeps playing between screens
		tick(g);
		handleControl(g);
		if (g->is_ticked) {
			UpdateFrame(g);
		}
		DrawFrame(g);
	}

	deinit(g);

	return 0;
}

