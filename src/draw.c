#include "game.h"
#include "resource.h"
#include "screen.h"
#include "draw.h"

#define W_TILE_LAST_NUM (W_TILE_COUNT - 1)
#define H_TILE_LAST_NUM (H_TILE_COUNT - 1)

#define XX(_x) (_x + c->level_start_x)
#define XY(_y) (_y + c->level_start_y)




static void drawEat(struct main_screen_ctx *c) {
	struct level l = c->levels[c->curr_level];
	for (int i = 0; i < l.eat_count; i++) {
		struct eat eat = l.eats[i];
		if (eat.st != EAT_LIVE) {
			continue;
		}
		DrawRectangle(XX(eat.x * GRID_PX + EAT_PX_SZ/2), XY(eat.y * GRID_PX + EAT_PX_SZ/2), EAT_PX_SZ, EAT_PX_SZ, EAT_COLOR);
	}
}

static void drawScore(struct main_screen_ctx *c) {
	DrawText("Score:", 40, 40, 40, DARKGRAY);
	DrawText(TextFormat("%d", c->score), 190, 40, 40, RED);
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

static void draw_level_grid(struct level l, int x_ofs, int y_ofs) {
	for (int i = 0; i < l.w_tile_count + 1; i++) {
		DrawLine(i * GRID_PX + x_ofs, 0 + y_ofs, i * GRID_PX + x_ofs, l.h_tile_count * GRID_PX + y_ofs, LIGHTGRAY);
	}
	for (int i = 0; i < l.h_tile_count + 1; i++) {
		DrawLine(0 + x_ofs, i * GRID_PX + y_ofs, l.w_tile_count * GRID_PX + x_ofs, i * GRID_PX + y_ofs, LIGHTGRAY);
	}
}

static void draw_next_level_portal(struct main_screen_ctx *c) {
	struct next_level_portal nl_portal =  c->levels[c->curr_level].nl_portal;
	DrawRectangle(XX(nl_portal.x * GRID_PX), XY(nl_portal.y * GRID_PX), GRID_PX, GRID_PX, GREEN);
}

static void draw_level(struct main_screen_ctx *c) {
	struct level l = c->levels[c->curr_level];
	int level_w = l.w_tile_count * GRID_PX;
	int level_h = l.h_tile_count * GRID_PX;

	c->level_start_x = (SCREEN_WIDTH - level_w)/2;
	c->level_start_y = (SCREEN_HEIGHT - level_h)/2;

	draw_level_grid(l, c->level_start_x, c->level_start_y);

	if (c->is_open_next_level_portal) {
		draw_next_level_portal(c);
	}
}

void DrawFrame(union screen_ctx *ctx) {
	struct main_screen_ctx *c = &ctx->main_sc;

	//DrawLogoScreen();
	//DrawFPS(10, 10);
	draw_level(c);
	snake_draw(&c->sn, c->level_start_x, c->level_start_y);
	drawEat(c);
	drawScore(c);
	if (c->is_game_over) {
		//TraceLog(LOG_WARNING, "is_game_over", GRID_W_COUNT, GRID_H_COUNT);
		drawGameOver();
	}
}

