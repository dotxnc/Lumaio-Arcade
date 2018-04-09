#include "resource.h"

bool resource_loadshader(const char* vertex_file, const char* fragment_file, const char* ID)
{
    if (resource_manager.num_shaders > MAX_SHADERS-1) return false;
    resource_manager.shaders[resource_manager.num_shaders].shader = LoadShader(vertex_file, fragment_file);
    strcpy(resource_manager.shaders[resource_manager.num_shaders].ID, ID);
    resource_manager.num_shaders++;
    return true;
}

bool resource_loadmodel(const char* model_file, const char* texture_file, const char* ID)
{
    if (resource_manager.num_models > MAX_MODELS-1) return false;
    resource_manager.models[resource_manager.num_models].model = LoadModel(model_file);
    if (texture_file != NULL) {
        resource_manager.models[resource_manager.num_models].model.material.maps[MAP_DIFFUSE].texture = LoadTexture(texture_file);
    }
    strcpy(resource_manager.models[resource_manager.num_models].ID, ID);
    resource_manager.num_models++;
    return true;
}

bool resource_loadtexture(const char* texture_file, const char* ID)
{
    if (resource_manager.num_textures > MAX_TEXTURES-1) return false;
    resource_manager.textures[resource_manager.num_textures].texture = LoadTexture(texture_file);
    strcpy(resource_manager.textures[resource_manager.num_textures].ID, ID);
    resource_manager.num_textures++;
    return true;
}

Shader* resource_getshader(const char* ID)
{
    for (int i = 0; i < resource_manager.num_shaders; i++) {
        if (strcmp(resource_manager.shaders[i].ID, ID)==0) {
            return &resource_manager.shaders[i].shader;
        }
    }
    return NULL;
}

Model* resource_getmodel(const char* ID)
{
    for (int i = 0; i < resource_manager.num_models; i++) {
        if (strcmp(resource_manager.models[i].ID, ID)==0) {
            return &resource_manager.models[i].model;
        }
    }
    return NULL;
}

Texture2D* resource_gettexture(const char* ID)
{
    for (int i = 0; i < resource_manager.num_textures; i++) {
        if (strcmp(resource_manager.textures[i].ID, ID)==0) {
            return &resource_manager.textures[i].texture;
        }
    }
    return NULL;
}

void resource_free()
{
    for (int i = 0; i < resource_manager.num_shaders; i++) {
        UnloadShader(resource_manager.shaders[i].shader);
    }
    for (int i = 0; i < resource_manager.num_models; i++) {
        UnloadModel(resource_manager.models[i].model);
    }
    for (int i = 0; i < resource_manager.num_textures; i++) {
        UnloadTexture(resource_manager.textures[i].texture);
    }
}
