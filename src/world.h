#ifndef WORLD_H
#define WORLD_H

#include "util/hashmap.h"
#include "cabinet.h"
#include <raylib.h>
#include <raymath.h>

static hashmap_t world_arcades;
static Model world_model;

void world_initialize(const char*);
void world_update(float, Camera3D, bool*);
void world_draw();
void world_alt_draw();
void world_free();
hashmap_t* world_getarcades();

#endif
