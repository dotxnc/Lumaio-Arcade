#ifndef EDITORSTATE_H
#define EDITORSTATE_H

#include "../game.h"

void editor_init();
void editor_update(float);
void editor_draw();
void editor_quit();
void editor_enter(gamestate_t*);
void editor_leave(gamestate_t*);
gamestate_t* editor_getstate();

#endif
