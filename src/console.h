#ifndef CONSOLE_H
#define CONSOLE_H

#include <stdbool.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

static bool console_open = false;
static char console_output[512][128];
static int console_output_size;
static lua_State* L;

void console_init();
void console_reload();
void console_update();
void console_draw();

#endif
