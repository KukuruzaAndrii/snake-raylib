#include <assert.h>

#include "raylib.h"

#include "game.h"

#include "draw.h"

#define countof(_a) ((sizeof _a)/(sizeof _a[0]))

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

static Font font = { 0 };

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

void DrawFrame(struct game_ctx *g, Font *_font) {
	font = *_font;
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

