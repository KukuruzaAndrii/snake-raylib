#include "game.h"

#include "screen.h"
#include "draw.h"
#include "resource.h"

#define W_TILE_COUNT (SCREEN_WIDTH/GRID_PX)
#define H_TILE_COUNT (SCREEN_HEIGHT/GRID_PX)

#define GRID_W_LINE_COUNT (W_TILE_COUNT + 1)
#define GRID_H_LINE_COUNT (H_TILE_COUNT + 1)

#define SCREEN_W_OFFSET ((SCREEN_WIDTH - W_TILE_COUNT * GRID_PX)/2)
#define SCREEN_H_OFFSET ((SCREEN_HEIGHT - H_TILE_COUNT * GRID_PX)/2)

#define X(_x) (_x + SCREEN_W_OFFSET)
#define Y(_y) (_y + SCREEN_H_OFFSET)

#define GAME_LOGO_OFFSET_PS (.4)

#define LABEL_GAME_LOGO_SIZE 220

//todo delete
#define SNAKE_BODY_COLOR SKYBLUE
#define SNAKE_HEAD_COLOR BLUE

#define ST_SC_SNAKE_MOVE_SZ 4
enum dir st_sc_sequence[] = {DIR_RIGHT, DIR_UP, DIR_RIGHT, DIR_DOWN};

static void start_screen_init(union screen_ctx *ctx);
static enum screen_type start_screen_handle_input(union screen_ctx *ctx);
static void start_screen_update(union screen_ctx *ctx);
static void start_screen_draw(union screen_ctx *ctx);

struct screen start_screen = {
	.init = start_screen_init,
	.handle_input = start_screen_handle_input,
	.update = start_screen_update,
	.draw = start_screen_draw,
};

static void start_screen_init(union screen_ctx *ctx) {
	struct start_screen_ctx *c = ctx->sc_ctx;
	c->start_screen_snake_x = -4;
	c->start_screen_snake_y = 0;
	c->st_sc_state_cur = 0;
	c->st_sc_cntr = 0;
	c->menu_count = 2;
	c->selected_menu = 0;
}

static void start_screen_update(union screen_ctx *ctx) {
	struct start_screen_ctx *c = ctx->sc_ctx;
	if (c->st_sc_cntr == ST_SC_SNAKE_MOVE_SZ) {
		c->st_sc_state_cur++;
	}

	enum dir dir = st_sc_sequence[c->st_sc_state_cur];
	if (dir == DIR_RIGHT) {
		c->start_screen_snake_x++;
	} else if (dir == DIR_UP) {
		c->start_screen_snake_y++;
	} else if (dir == DIR_DOWN) {
		c->start_screen_snake_y--;
	}

	c->st_sc_cntr++;
}

static enum screen_type start_screen_handle_input(union screen_ctx *ctx) {
	struct start_screen_ctx *c = ctx->sc_ctx;
	if (IsKeyPressed(KEY_DOWN)) {
		if (c->selected_menu == c->menu_count - 1) {
			c->selected_menu = 0;
		} else {
			c->selected_menu++;
		}
	} else if (IsKeyPressed(KEY_UP)) {
		if (c->selected_menu == 0) {
			c->selected_menu = c->menu_count - 1;
		} else {
			c->selected_menu--;
		}
	} else if (IsKeyPressed(KEY_ENTER)) {
		if (c->selected_menu == 0) {
			return SCREEN_GAME;
		} else if (c->selected_menu == 1) {
			return SCREEN_EXIT;
		}
	}
	return SCREEN_START;
}

static void drawGridFullScreen(void) {
	for (int i = 0; i < GRID_W_LINE_COUNT; i++) {
		DrawLine(X(i * GRID_PX), Y(0), X(i * GRID_PX), Y(H_TILE_COUNT * GRID_PX), LIGHTGRAY);
	}
	for (int i = 0; i < GRID_H_LINE_COUNT; i++) {
		DrawLine(X(0), Y(i * GRID_PX), X(W_TILE_COUNT * GRID_PX), Y(i * GRID_PX), LIGHTGRAY);
	}
}

static void drawGameLogo(void) {
	const char* label = "SnaKe HiKe Game";

	int game_logo_boundary_h = SCREEN_HEIGHT * GAME_LOGO_OFFSET_PS;
	Vector2 label_sz = MeasureTextEx(font, label, LABEL_GAME_LOGO_SIZE, 0);
	Vector2 label_pos = {.x = (SCREEN_WIDTH - label_sz.x) / 2, .y = (game_logo_boundary_h - label_sz.y) / 2};
	DrawTextEx(font, label, label_pos, LABEL_GAME_LOGO_SIZE, 0, RED);
	// DrawRectangleLines(0, (game_logo_boundary_h - label_sz.y)/2, SCREEN_WIDTH, label_sz.y, RED);
}

static void drawGameMenu(struct start_screen_ctx *c) {
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
		Vector2 label_pos = {.x = (SCREEN_WIDTH - label_sz.x) / 2, .y = label_sz.y / 2 + menu_start_y +
																		menu_div_space * i};
		DrawTextEx(font, label, label_pos, LABEL_GAME_OVER_SIZE, 0, DARKGRAY);

		// selected menu item
		if (c->selected_menu == i) {
			int label_div_space = 40;
			DrawRectangle((SCREEN_WIDTH - label_sz.x) / 2 - label_div_space,
						  label_sz.y + menu_start_y + menu_div_space * i - EAT_PX_SZ / 2, EAT_PX_SZ, EAT_PX_SZ,
						  EAT_COLOR);
		}
	}
}

static void drawGameMenuSnake(struct start_screen_ctx *c) {
	//DrawRectangleLines(0,0,SCREEN_WIDTH,  SCREEN_HEIGHT * GAME_LOGO_OFFSET_PS, RED);
	//DrawRectangleLines(0,SCREEN_HEIGHT * GAME_LOGO_OFFSET_PS, SCREEN_WIDTH, SCREEN_HEIGHT * (1 - GAME_LOGO_OFFSET_PS), RED);
	int snake_x = c->start_screen_snake_x;
	int snake_y = c->start_screen_snake_y;
	int chanks = 5;
	for (int i = 0; i < chanks; i++) {
		Color chank_c = SNAKE_BODY_COLOR;
		if (i == 0) {
			chank_c = SNAKE_HEAD_COLOR;
		}
		DrawRectangle((snake_x - i) * GRID_PX, SCREEN_HEIGHT * GAME_LOGO_OFFSET_PS + snake_y * GRID_PX - GRID_PX / 2,
					  GRID_PX, GRID_PX, chank_c);
	}

}

static void drawStart(struct start_screen_ctx *c) {
	drawGameLogo();
	drawGameMenu(c);
	drawGameMenuSnake(c);
}

static void start_screen_draw(union screen_ctx *ctx) {
	struct start_screen_ctx *c = ctx->sc_ctx;
	drawGridFullScreen();
	drawStart(c);
}
