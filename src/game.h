#ifndef GAME_H
#define GAME_H

#include <stdbool.h>

struct game_t;
struct gamestate_t;

typedef void(*state_init)     ();
typedef void(*state_update)   (float);
typedef void(*state_draw)     ();
typedef void(*state_alt_draw) ();
typedef void(*state_ui_draw)  ();
typedef void(*state_quit)     ();
typedef void(*state_enter)    (struct gamestate_t*);
typedef void(*state_leave)    (struct gamestate_t*);

typedef struct gamestate_t {
    struct game_t* game_ptr;
    
    state_init     game_init;
    state_update   game_update;
    state_draw     game_draw;
    state_alt_draw game_alt_draw;
    state_ui_draw  game_ui_draw;
    state_quit     game_quit;
    state_enter    game_enter;
    state_leave    game_leave;
    
    bool initialized;
} gamestate_t;

typedef struct game_t {
    gamestate_t* gamestate;
    int width;
    int height;
    void* title;
} game_t;

void game_init(game_t*, int, int, void*);
void game_setstate(game_t*, gamestate_t*);
void game_start(game_t*);
void game_shutdown(game_t*);

#endif
