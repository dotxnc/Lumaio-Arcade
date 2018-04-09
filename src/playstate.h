#ifndef PLAYSTATE_H
#define PLAYSTATE_H

#include "game.h"

void play_init();
void play_update(float);
void play_draw();
void play_quit();
void play_enter(gamestate_t*);
void play_leave(gamestate_t*);
gamestate_t* play_getstate();

#endif
