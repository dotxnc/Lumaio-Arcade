#include "game.h"

#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>

void game_init(game_t* game, int width, int height, void* title)
{
    game->width = width;
    game->height = height;
    game->title = title;
    
    game->gamestate = NULL;
    
    InitWindow(width, height, title);
    SetMousePosition((Vector2){0, 0});
    SetTargetFPS(60);
    SetExitKey(KEY_F12);
}

void game_setstate(game_t* game, gamestate_t* state)
{
    if (!state->initialized) {
        state->game_ptr = game;
        state->game_init();
        state->initialized = true;
    }
    
    if (game->gamestate != NULL) {
        game->gamestate->game_leave(state);
        state->game_enter(game->gamestate);
    }
    game->gamestate = state;
}

void game_start(game_t* game)
{
    while (!WindowShouldClose())
    {
        game->gamestate->game_update(GetFrameTime());
        BeginDrawing();
            ClearBackground(BLACK);
            game->gamestate->game_draw();
        EndDrawing();
    }
}

void game_shutdown(game_t* game)
{
    CloseWindow();
}
