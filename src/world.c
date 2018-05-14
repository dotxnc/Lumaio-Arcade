#include "world.h"

void world_initialize(const char* file)
{
    FILE* fp;
    char line[256];
    size_t len = 0;
    ssize_t read;
    
    fp = fopen(file, "r");
    if (fp == NULL) {
        printf("Failed to read map file\n");
        return;
    }
    
    while (fgets(line, 256, fp) != NULL) {
        if (line[0] == ']' || strlen(line) <= 1) continue;
        
        int type = 0;
        int index = 0;
        char* token = strtok(line, " ");
        char tokens[12][256] = {NULL};
        while (token != NULL) {
            if (token[strlen(token)-1] == '\n') token[strlen(token)-1] = '\0';
            strcpy(tokens[index], token);
            index++;
            token = strtok(NULL, " ");
        }
        free(token);
        printf("MAP RES: ");
        for (int i = 0; i < 12; i++) {
            printf("%s(%d) ", tokens[i] != NULL ? tokens[i] : "NULL", i);
        }
        printf("\n");
        
        if (strcmp(tokens[0], "mesh") == 0) {
            world_model = LoadModel(tokens[1]);
            SetModelShader(&world_model, *resource_getshader("gbuffer"));
            SetModelMap(&world_model, MAP_DIFFUSE, GetTextureDefault());
            SetModelMap(&world_model, MAP_SPECULAR, GetTextureDefault());
            SetModelMap(&world_model, MAP_NORMAL, GetTextureDefault());
            SetModelMap(&world_model, MAP_EMISSION, GetTransparentTexture());
        }
        else if (strcmp(tokens[0], "arcade") == 0) {
            cabinet_t a;// = malloc(sizeof(cabinet_t));
            cabinet_init(&a, &tokens[9], &tokens[8]);
            a.position = (Vector3){atof(tokens[1]), atof(tokens[2]), atof(tokens[3])};
            cabinet_rotate(&a, atof(tokens[4]), atof(tokens[5]), atof(tokens[6]));
            hashmap_push(&world_arcades, tokens[7], &a, sizeof(cabinet_t));
        }
        
    }
}

void world_update(float dt, Camera3D camera, bool* interacting)
{
    hashmap_t* current = world_getarcades();
    while (current != NULL) {
        cabinet_t* ptr = current->value;
        if (ptr != NULL) {
            cabinet_update(ptr);
            // interact
            if (IsKeyPressed(KEY_E)) {
                Ray r;
                r.position = camera.position;
                r.direction = Vector3Subtract(camera.target, camera.position);
                
                RayHitInfo h = GetCollisionRayModel(r, ptr->screen);
                if (h.hit) {
                    *interacting = true;
                    ptr->interacting = true;
                    // get arcade name
                    lua_getglobal(ptr->L, "arcade");
                    lua_getfield(ptr->L, -1, "name");
                    const char* arcade_name = lua_tostring(ptr->L, -1);
                    discordhelper_setarcade(arcade_name, -1);
                    break;
                }
            }
        }
        current = current->next;
    }
}

void world_draw()
{
    // Matrix m = MatrixIdentity();
    // m = MatrixMultiply(m, MatrixTranslate(0, 0, 0));
    // world_model.transform = m;
    DrawModel(world_model, Vector3Zero(), 1, WHITE);
    
    hashmap_t* current = world_getarcades();
    while (current != NULL) {
        cabinet_t* ptr = current->value;
        if (ptr != NULL) {
            cabinet_draw(ptr);
        }
        current = current->next;
    }
}

void world_alt_draw()
{
    hashmap_t* current = world_getarcades();
    while (current != NULL) {
        cabinet_t* ptr = current->value;
        if (ptr != NULL) {
            cabinet_drawgame(ptr);
        }
        current = current->next;
    }
}

void world_free()
{
    hashmap_t* current = world_getarcades();
    while (current != NULL) {
        cabinet_t* ptr = current->value;
        if (ptr != NULL) {
            cabinet_free(ptr);
        }
        current = current->next;
    }
}

hashmap_t* world_getarcades() {
    return &world_arcades;
}
