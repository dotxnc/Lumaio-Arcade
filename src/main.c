#include <stdio.h>
#include <stdlib.h>

#include "game.h"
#include "playstate.h"

#include "resource.h"

int main(int argc, char** argv)
{
    game_t game;
    game_init(&game, 1280, 720, "Arcade");
    game_setstate(&game, play_getstate());
    game_start(&game);
    game_shutdown(&game);
    
    resource_free();
    
    play_quit();
    
    return 0;
}
