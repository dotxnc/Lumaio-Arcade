#ifndef CABINET_H
#define CABINET_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <raylib.h>
#include <raymath.h>
#ifdef __unix__
    #include <lua5.3/lua.h>
#else
    #include <lua.h>
#endif

#include "resource.h"

typedef struct cabinet_t {
    Model* machine;
    Model* screen;
    Vector3 position;
    Matrix rotation;
    
    RenderTexture2D target;
    RenderTexture2D temp;
    
    bool interacting;
    bool errored;
    
    lua_State* L;
    char* script_file;
} cabinet_t;

void cabinet_init(cabinet_t*, const char*, const char*);
void cabinet_rotate(cabinet_t*, float, float, float);
void cabinet_update(cabinet_t*);
void cabinet_draw(cabinet_t*);
void cabinet_drawgame(cabinet_t*);
void cabinet_free(cabinet_t*);

#endif
