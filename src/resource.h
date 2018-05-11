#ifndef RESOURCE_H
#define RESOURCE_H

#include <raylib.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "util/raylibext.h"

#define MAX_ID 32
#define MAX_SHADERS 32
#define MAX_MODELS 32
#define MAX_TEXTURES 32

typedef struct res_metadata_t {
    char type[256];
    char uid[256];
    char var1[256];
    char var2[256];
} res_metadata_t;

typedef struct res_shader_t {
    Shader shader;
    char ID[MAX_ID];
} res_shader_t;

typedef struct res_model_t {
    Model model;
    char ID[MAX_ID];
} res_model_t;

typedef struct res_texture_t {
    Texture2D texture;
    char ID[MAX_ID];
} res_texture_t;

typedef struct ResourceManager_t {
    res_shader_t shaders[MAX_SHADERS];
    res_model_t models[MAX_MODELS];
    res_texture_t textures[MAX_TEXTURES];
    
    int num_shaders;
    int num_models;
    int num_textures;
} ResourceManager_t;

static ResourceManager_t resource_manager;

void resource_loadmetadata(const char*);
bool resource_loadshader(const char*, const char*, const char*);
bool resource_loadmodel(const char*, const char*, const char*);
bool resource_loadtexture(const char*, const char*);
Shader* resource_getshader(const char*);
Model* resource_getmodel(const char*);
Texture2D* resource_gettexture(const char*);
void resource_free();

#endif
