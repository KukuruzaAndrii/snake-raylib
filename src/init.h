#pragma once

struct game_ctx* init_game(void);

void reset_game(struct game_ctx* g);
void reset_level(struct game_ctx* g);
union screen_ctx;
void init_main_screen (union screen_ctx *ctx);
