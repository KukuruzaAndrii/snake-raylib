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
#include <assert.h>
#include <string.h>

#include "raylib.h"
#include "screen_logo.h"

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


#define XX(_x) (_x + g->level_start_x)
#define XY(_y) (_y + g->level_start_y)


#define EAT_COLOR MAGENTA

#define SNAKE_BODY_COLOR SKYBLUE
#define SNAKE_HEAD_COLOR BLUE

#define SNAKE_GAME_OVER_COLOR RED
#define SNAKE_EAT_COLOR MAGENTA

#define LABEL_GAME_OVER_SIZE 190
#define LABEL_GAME_LOGO_SIZE 220

#define GAME_LOGO_OFFSET_PS (.4)


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

enum eat_state {
	EAT_NO_EAT = 0,
	EAT_LIVE,
};

struct eat {
	int x;
	int y;
	enum eat_state st;
};

struct next_level_portal {
	int x;
	int y;
};

struct level {
	int w_tile_count;
	int h_tile_count;
	int eat_count;
	struct eat eats[10];
	struct next_level_portal nl_portal;
};


struct game_ctx {
	struct node * head;
	enum dir dir;
	int score;
	unsigned is_ticked:1;
	unsigned is_game_over:1;
	unsigned is_eat:1;
	unsigned is_was_pressed_before_tick:1;
	unsigned is_start_screen:1;
	unsigned is_exit:1;
	int start_screen_snake_x;
	int menu_count;
	int selected_menu;
	struct level levels[3];
	int curr_level;
	int already_eat_count;
	unsigned is_open_next_level_portal:1;
	unsigned is_warping_to_next_level:1;

	int level_start_x;
	int level_start_y;
};



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

static void remove_head(struct game_ctx *g) {
	if (g->head->next) {
		g->head = g->head->next;
	}
}

/*
static void updateSnake(struct game_ctx *g) {
}

static Vector2 get_next_eat() {
	return (struct Vector2){
		.x = GetRandomValue(0, W_TILE_LAST_NUM),
		.y = GetRandomValue(0, H_TILE_LAST_NUM),
	};
}
*/

static unsigned checkEat(struct game_ctx *g) {
	struct level* l = &(g->levels[g->curr_level]);
	for (int i = 0; i < l->eat_count; i++) {
		struct eat *eat = &(l->eats[i]);
		if (eat->st != EAT_LIVE) {
			continue;
		}
		if (g->head->x == eat->x && g->head->y == eat->y) {
			//struct Vector2 next_eat_pos = get_next_eat();
			//while (isContainsNodeByVal(g->head, next_eat_pos.x, next_eat_pos.y)) {
			//	next_eat_pos = get_next_eat();
			//}
			//g->eat.x = next_eat_pos.x;
			//g->eat.y = next_eat_pos.y;
			eat->st = EAT_NO_EAT;
			g->score += 1;
			g->already_eat_count += 1;
			return 1;
		}
	}
	return 0;
}

static unsigned check_game_over(struct game_ctx *g) {
	struct Vector2 next_head_pos = get_next_head_pos(g->head, g->dir);
	struct level l = g->levels[g->curr_level];

	// level borders
	if (next_head_pos.x == l.w_tile_count || next_head_pos.y == l.h_tile_count || next_head_pos.x < 0 || next_head_pos.y < 0) {
		return 1;
	}
	// self-eat
	if (isContainsNodeByVal(g->head, next_head_pos.x, next_head_pos.y)) {
		return 1;
	}

	return 0;
}

static void updateStartScreen(struct game_ctx *g) {
	g->start_screen_snake_x++;
}

static void UpdateFrame(struct game_ctx *g) {
	if (g->is_start_screen) {
		updateStartScreen(g);
		return;
	}

	g->is_game_over = check_game_over(g);
	if (g->is_game_over) {
		return;
	}

	g->head = growSnake(g);

	g->is_eat = checkEat(g);
	if (!g->is_eat) {
		shrinkSnake(g);
	}
	if (g->already_eat_count == g->levels[g->curr_level].eat_count) {
		g->is_open_next_level_portal = 1;
	}
	if (g->is_open_next_level_portal) {
		if (g->head->x == g->levels[g->curr_level].nl_portal.x &&
		    g->head->y == g->levels[g->curr_level].nl_portal.y) {
			g->is_warping_to_next_level = 1;
		}
	}

	if (g->is_warping_to_next_level) {
		remove_head(g);
	}
}

static void drawGridFullScreen(void) {
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
	if (g->is_warping_to_next_level) {
		head_c = SNAKE_BODY_COLOR;
	}

	DrawRectangle(XX(g->head->x * GRID_PX), XY(g->head->y * GRID_PX), GRID_PX, GRID_PX, head_c);
	foreach_node(g->head->next, n) {
		// TraceLog(LOG_WARNING, "x=%d y=%d", n->x, n->y);
		DrawRectangle(XX(n->x * GRID_PX), XY(n->y * GRID_PX), GRID_PX, GRID_PX, body_c);
	}
}

static void drawEat(struct game_ctx *g) {
	struct level l = g->levels[g->curr_level];
	for (int i = 0; i < l.eat_count; i++) {
		struct eat eat = l.eats[i];
		if (eat.st != EAT_LIVE) {
			continue;
		}
		DrawRectangle(XX(eat.x * GRID_PX + EAT_PX_SZ/2), XY(eat.y * GRID_PX + EAT_PX_SZ/2), EAT_PX_SZ, EAT_PX_SZ, EAT_COLOR);
	}
}

static void drawScore(struct game_ctx *g) {
	DrawText("Score:", 40, 40, 40, DARKGRAY);
	DrawText(TextFormat("%d", g->score), 190, 40, 40, RED);
}

static void drawGameOver(void) {
	const char *label = "GAME OVER";
	int spacer = 9;
	Vector2 label_sz = MeasureTextEx(font, label, LABEL_GAME_OVER_SIZE, spacer);
	Vector2 label_sz2 = MeasureTextEx(font, label, LABEL_GAME_OVER_SIZE + 20, 0);
	Vector2 label_pos = {.x = (SCREEN_WIDTH - label_sz.x)/2, .y = (SCREEN_HEIGHT - label_sz.y)/2};
	Vector2 label_pos2 = {.x = (SCREEN_WIDTH - label_sz2.x)/2, .y = (SCREEN_HEIGHT - label_sz2.y)/2};
	DrawTextEx(font, label, label_pos2, LABEL_GAME_OVER_SIZE + 20, 0, RED);
	DrawTextEx(font, label, label_pos, LABEL_GAME_OVER_SIZE, spacer, DARKGRAY);
}
static void drawGameLogo(void) {
	const char* label = "SnaKe HiKe Game";

	int game_logo_boundary_h = SCREEN_HEIGHT * GAME_LOGO_OFFSET_PS;
	Vector2 label_sz = MeasureTextEx(font, label, LABEL_GAME_LOGO_SIZE, 0);
	Vector2 label_pos = {.x = (SCREEN_WIDTH - label_sz.x)/2, .y = (game_logo_boundary_h - label_sz.y)/2};
	DrawTextEx(font, label, label_pos, LABEL_GAME_LOGO_SIZE, 0, RED);
	// DrawRectangleLines(0, (game_logo_boundary_h - label_sz.y)/2, SCREEN_WIDTH, label_sz.y, RED);
}

static void drawGameMenu(struct game_ctx *g) {
	const char* labels[] = {"Start", "Exit"};
	int lb_count = countof(labels);
	int menu_div_space = 250;
	Vector2 menu_label_sz = MeasureTextEx(font, "lol", LABEL_GAME_OVER_SIZE, 9); // need for height
	assert (lb_count > 0);
	int game_logo_boundary_h = SCREEN_HEIGHT * GAME_LOGO_OFFSET_PS;
	int game_menu_boundary_h = SCREEN_HEIGHT - game_logo_boundary_h;
	int total_menu_height = (lb_count - 1) * menu_div_space + lb_count * menu_label_sz.y;
	int menu_start_y = game_logo_boundary_h + (game_menu_boundary_h - total_menu_height) / 2;
	for (int i = 0; i < lb_count; i++) {
		const char* label = labels[i];
		Vector2 label_sz = MeasureTextEx(font, label, LABEL_GAME_OVER_SIZE, 9);
		// DrawRectangleLines(0, menu_start_y + label_sz.y/2 + menu_div_space * i, SCREEN_WIDTH, label_sz.y, RED);
		Vector2 label_pos = {.x = (SCREEN_WIDTH - label_sz.x)/2, .y = label_sz.y/2 + menu_start_y + menu_div_space * i};
		DrawTextEx(font, label, label_pos, LABEL_GAME_OVER_SIZE, 0, DARKGRAY);

		// selected menu item
		if (g->selected_menu == i) {
			int label_div_space = 40;
			DrawRectangle((SCREEN_WIDTH - label_sz.x)/2 - label_div_space,  label_sz.y + menu_start_y + menu_div_space * i - EAT_PX_SZ/2, EAT_PX_SZ, EAT_PX_SZ, EAT_COLOR);
		}
	}
}

static void drawGameMenuSnake(struct game_ctx *g) {
	//DrawRectangleLines(0,0,SCREEN_WIDTH,  SCREEN_HEIGHT * GAME_LOGO_OFFSET_PS, RED);
	//DrawRectangleLines(0,SCREEN_HEIGHT * GAME_LOGO_OFFSET_PS, SCREEN_WIDTH, SCREEN_HEIGHT * (1 - GAME_LOGO_OFFSET_PS), RED);
	int snake_x = g->start_screen_snake_x;
	int snake_y = SCREEN_HEIGHT * GAME_LOGO_OFFSET_PS;
	int chanks = 5;
	for (int i = 0; i < chanks; i++) {
		Color chank_c = SNAKE_BODY_COLOR;
		if (i == 0) {
			chank_c = SNAKE_HEAD_COLOR;
		}
		DrawRectangle((snake_x - i) * GRID_PX, snake_y - GRID_PX/2, GRID_PX, GRID_PX, chank_c);
	}

}

static void drawStart(struct game_ctx *g) {
	drawGameLogo();
	drawGameMenu(g);
	drawGameMenuSnake(g);
}

static void draw_level_grid(struct level l, int x_ofs, int y_ofs) {
	for (int i = 0; i < l.w_tile_count + 1; i++) {
		DrawLine(i * GRID_PX + x_ofs, 0 + y_ofs, i * GRID_PX + x_ofs, l.h_tile_count * GRID_PX + y_ofs, LIGHTGRAY);
	}
	for (int i = 0; i < l.h_tile_count + 1; i++) {
		DrawLine(0 + x_ofs, i * GRID_PX + y_ofs, l.w_tile_count * GRID_PX + x_ofs, i * GRID_PX + y_ofs, LIGHTGRAY);
	}
}

static void draw_next_level_portal(struct game_ctx *g) {
	struct next_level_portal nl_portal =  g->levels[g->curr_level].nl_portal;
	DrawRectangle(XX(nl_portal.x * GRID_PX), XY(nl_portal.y * GRID_PX), GRID_PX, GRID_PX, GREEN);
}

static void draw_level(struct game_ctx *g) {
	struct level l = g->levels[g->curr_level];
	int level_w = l.w_tile_count * GRID_PX;
	int level_h = l.h_tile_count * GRID_PX;

	g->level_start_x = (SCREEN_WIDTH - level_w)/2;
	g->level_start_y = (SCREEN_HEIGHT - level_h)/2;

	draw_level_grid(l, g->level_start_x, g->level_start_y);

	if (g->is_open_next_level_portal) {
		draw_next_level_portal(g);
	}
}

static void DrawFrame(struct game_ctx *g) {
	BeginDrawing();

	ClearBackground(RAYWHITE);
	//DrawLogoScreen();
	//DrawFPS(10, 10);
	if (g->is_start_screen) {
		drawGridFullScreen();
		drawStart(g);
		EndDrawing();
		return;
	}
	draw_level(g);
	drawSnake(g);
	drawEat(g);
	drawScore(g);
	if (g->is_game_over) {
		//TraceLog(LOG_WARNING, "is_game_over", GRID_W_COUNT, GRID_H_COUNT);
		drawGameOver();
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
	while (!WindowShouldClose() && !g->is_exit) {
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

