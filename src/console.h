#ifndef CONSOLE_H
#define CONSOLE_H

#include <stdbool.h>
#include <string.h>

#include <raylib.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#define CONSOLE_LENGTH 1024*(128+1) // 1024 lines. 128 length + null

static bool console_open = false;
static char console_output[CONSOLE_LENGTH];
static int console_output_length;
static char console_input[128+1];
static int console_input_length;
static lua_State* console_L;
static int console_num = 0;

static RenderTexture2D console_buffer;

// bind functions
int lua_Conout(lua_State*);

// console lua functions
bool console_get(const char*);
void console_pushstring(const char*);
void console_pushnumber(lua_Number);
const char* console_call();

// console functions
void console_init();
void console_print(const char*);
void console_reload();
void console_update();
void console_draw();
void console_toggle();

#endif
