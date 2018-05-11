#ifndef CABINET_H
#define CABINET_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <raylib.h>
#include <raymath.h>
#ifdef __unix__
    #include <lua5.3/lua.h>
    #include <lua5.3/lua.h>
    #include <lua5.3/lauxlib.h>
    #include <lua5.3/lualib.h>
#else
    #include <lua.h>
    #include <lauxlib.h>
    #include <lualib.h>
#endif

#include "resource.h"
#include "world.h"
#include "util/hashmap.h"
#include "console.h"

typedef struct cabinet_t {
    Model* machine;
    Model* screen;
    Vector3 position;
    Matrix rotation;
    
    Shader* shader;
    
    RenderTexture2D target;
    RenderTexture2D temp;
    
    bool interacting;
    bool errored;
    
    lua_State* L;
    char script_file[128];
    
    unsigned int hash;
} cabinet_t;

static Matrix old_machine;
static Matrix old_screen;

void cabinet_init(cabinet_t*, const char*, const char*);
void cabinet_reload(cabinet_t*);
void cabinet_rotate(cabinet_t*, float, float, float);
void cabinet_setshader(cabinet_t*, const char*);
void cabinet_update(cabinet_t*);
void cabinet_draw(cabinet_t*);
void cabinet_drawgame(cabinet_t*);
void cabinet_free(cabinet_t*);

#endif
