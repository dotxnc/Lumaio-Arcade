#include <stdio.h>
#include <stdlib.h>

#include "util/discordhelper.h"

#include "game.h"
#include "states/playstate.h"
#include "states/editorstate.h"

#include "resource.h"

#include "util/crc32.h"

int main(int argc, char** argv)
{
    discordhelper_initialize();
    
    game_t game;
    game_init(&game, 1280, 720, "Arcade");
    game_setstate(&game, play_getstate());
    game_start(&game);
    game_shutdown(&game);
    
    resource_free();
    
    play_quit();
    editor_quit();
    
    discordhelper_shutdown();
    
    return 0;
}
