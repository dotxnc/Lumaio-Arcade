#include <stdio.h>
#include <stdlib.h>

#include "game.h"
#include "states/playstate.h"
#include "states/editorstate.h"

#include "resource.h"

#include "util/crc32.h"

int main(int argc, char** argv)
{
    game_t game;
    game_init(&game, 1280, 720, "Arcade");
    game_setstate(&game, play_getstate());
    game_start(&game);
    game_shutdown(&game);
    
    resource_free();
    
    play_quit();
    editor_quit();
    
    char colors[][32] = {
        "red",
        "green",
        "blue",
        "yellow",
        "cyan",
        "purple",
        "white",
        "black"
    };
    for (int i = 0; i < 8; i++) {
        unsigned int h = xcrc32(&colors[i], strlen(colors[i]), 0xffffffff);
        char a[9];
        sprintf(a, "%u", h);
        printf("[%-6s] : [%10s] : [%d]\n", colors[i], a, (int)strlen(colors[i]));
    }
    
    return 0;
}
