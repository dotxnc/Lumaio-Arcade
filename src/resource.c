#include "resource.h"

void resource_loadmetadata(const char* mapfile)
{
    FILE* fp;
    char line[256];
    size_t len = 0;
    ssize_t read;
    
    fp = fopen(mapfile, "r");
    if (fp == NULL) {
        printf("Failed to load resource metadata file\n");
    }
    
    while (fgets(line, 256, fp) != NULL) {
        if (line[0] == ']' || strlen(line) <= 1) continue;
        int type = 0;
        int index = 0;
        char* token = strtok(line, " ");
        char metadata[4][256];
        while (token != NULL) {
            if (token[strlen(token)-1] == '\n') token[strlen(token)-1] = '\0';
            strcpy(metadata[index], token);
            index++;
            token = strtok(NULL, " ");
        }
        free(token);
        
        if (strcmp(metadata[0], "model") == 0) {
            resource_loadmodel(metadata[2], metadata[3], metadata[1]);
        }
        else if (strcmp(metadata[0], "texture") == 0) {
            resource_loadtexture(metadata[2], metadata[1]);
        }
        else if (strcmp(metadata[0], "shader") == 0) {
            resource_loadshader(metadata[2], metadata[3], metadata[1]);
        }
        else if (strcmp(metadata[0], "fshader") == 0) {
            resource_loadshader(NULL, metadata[2], metadata[1]);
        }
        else if (strcmp(metadata[0], "vshader") == 0) {
            resource_loadshader(metadata[2], NULL, metadata[1]);
        }
        
        printf("LOAD RESOURCE: %s %s %s\n", metadata[0], metadata[1], metadata[2]);
    }
}

bool resource_loadshader(const char* vertex_file, const char* fragment_file, const char* ID)
{
    if (resource_manager.num_shaders > MAX_SHADERS-1) return false;
    if (resource_getshader(ID) != NULL) return false;
    resource_manager.shaders[resource_manager.num_shaders].shader = LoadShader(vertex_file, fragment_file);
    strcpy(resource_manager.shaders[resource_manager.num_shaders].ID, ID);
    resource_manager.num_shaders++;
    return true;
}

bool resource_loadmodel(const char* model_file, const char* texture_file, const char* ID)
{
    if (resource_manager.num_models > MAX_MODELS-1) return false;
    if (resource_getmodel(ID) != NULL) return false;
    resource_manager.models[resource_manager.num_models].model = LoadModel(model_file);
    if (texture_file != NULL) {
        resource_manager.models[resource_manager.num_models].model.material.maps[MAP_DIFFUSE].texture = LoadTexture(texture_file);
    } else {
        resource_manager.models[resource_manager.num_models].model.material.maps[MAP_DIFFUSE].texture = GetTextureDefault();
    }
    resource_manager.models[resource_manager.num_models].model.material.maps[MAP_SPECULAR].texture = GetTextureDefault();
    resource_manager.models[resource_manager.num_models].model.material.maps[MAP_NORMAL].texture = GetTextureDefault();
    resource_manager.models[resource_manager.num_models].model.material.maps[MAP_EMISSION].texture = GetTransparentTexture();
    strcpy(resource_manager.models[resource_manager.num_models].ID, ID);
    resource_manager.num_models++;
    return true;
}

bool resource_loadtexture(const char* texture_file, const char* ID)
{
    if (resource_manager.num_textures > MAX_TEXTURES-1) return false;
    if (resource_gettexture(ID) != NULL) return false;
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
