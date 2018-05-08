#include "playstate.h"
#include <raylib.h>
#include "../resource.h"
#include "../cabinet.h"
#include "../console.h"
#include "../util/raylibext.h"

static cabinet_t arcade1;
static cabinet_t arcade2;
static cabinet_t arcade3;
static Camera camera;
static bool interacting = false;
static Vector2 save_mouse = (Vector2){0, 0};
static Vector3 light_position = (Vector3){1.5, 3, 2};

static Shader* gbuffer;
static Shader* lighting;

static Model ground;

void play_init()
{
    console_init();
    
    gbuffer = resource_getshader("gbuffer");
    gbuffer->locs[LOC_MATRIX_MODEL] = GetShaderLocation(*gbuffer, "modelMatrix");
    
    lighting = resource_getshader("lighting");
    
    SetModelShader(resource_getmodel("arcade1"), *gbuffer);
    SetModelShader(resource_getmodel("arcade2"), *gbuffer);
    
    SetModelShader(resource_getmodel("arcade1_screen"), *gbuffer);
    SetModelShader(resource_getmodel("arcade2_screen"), *gbuffer);
    
    cabinet_init(&arcade1, "arcade1", "assets/scripts/games/test.lua");
    cabinet_init(&arcade2, "arcade2", "assets/scripts/games/snake.lua");
    cabinet_init(&arcade3, "arcade2", "assets/scripts/games/space_invaders.lua");
    
    cabinet_rotate(&arcade1, 0, -0.3, 0);
    cabinet_rotate(&arcade2, 0, 0.3, 0);
    cabinet_rotate(&arcade3, 0, 0.1, 0);
    
    cabinet_setshader(&arcade2, "vignette");
    cabinet_setshader(&arcade3, "vignette");
    
    arcade2.position = (Vector3){3, 0, -0.75};
    arcade3.position = (Vector3){-3, 0, -0.75};
    
    ground = LoadModelFromMesh(GenMeshPlane(100, 100, 5, 5));
    ground.material.shader = *gbuffer;
    SetModelMap(&ground, MAP_DIFFUSE, GetTextureDefault());
    SetModelMap(&ground, MAP_SPECULAR, GetTextureDefault());
    SetModelMap(&ground, MAP_NORMAL, GetTextureDefault());
    
    hashmap_pushvalue(world_getarcades(), "test", &arcade1);
    hashmap_pushvalue(world_getarcades(), "snake", &arcade2);
    hashmap_pushvalue(world_getarcades(), "invaders", &arcade3);
    
    camera = (Camera){{0, 3.5, 5}, {0, 3, 0}, {0, 1, 0}, 90.f};
    SetCameraMode(camera, CAMERA_FIRST_PERSON);
}

void play_update(float dt)
{
    
    lua_State* cl = console_getstate();
    
    lua_newtable(cl); // CAMERA
    
    lua_newtable(cl); // POSITON
        lua_pushnumber(cl, camera.position.x);
        lua_rawseti(cl, -2, 1);
        lua_pushnumber(cl, camera.position.y-3.5);
        lua_rawseti(cl, -2, 2);
        lua_pushnumber(cl, camera.position.z);
        lua_rawseti(cl, -2, 3);
    lua_setfield(cl, -2, "position");
    
    lua_newtable(cl); // TARGET
        lua_pushnumber(cl, camera.position.x);
        lua_rawseti(cl, -2, 1);
        lua_pushnumber(cl, camera.position.y-3.5);
        lua_rawseti(cl, -2, 2);
        lua_pushnumber(cl, camera.position.z);
        lua_rawseti(cl, -2, 3);
    lua_setfield(cl, -2, "target");
    
    lua_setglobal(cl, "camera");
    
    SetShaderVector3(*lighting, "viewpos", camera.position);
    
    if (IsKeyPressed(KEY_GRAVE)) {
        console_toggle();
    }
    
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
                    interacting = true;
                    ptr->interacting = true;
                    break;
                }
            }
        }
        current = current->next;
    }
    
    console_update();
    
    if (console_isopen()) return;
    
    if (!interacting) {
        UpdateCamera(&camera);
        save_mouse = GetMousePosition();
    } else {
        if (IsKeyPressed(KEY_ESCAPE)) {
            interacting = false;
            SetMousePosition(save_mouse);
        }
    }
    
}

void play_draw()
{
    Begin3dMode(camera);
        DrawModel(ground, Vector3Zero(), 1, WHITE);
        hashmap_t* current = world_getarcades();
        while (current != NULL) {
            cabinet_t* ptr = current->value;
            if (ptr != NULL) {
                cabinet_draw(ptr);
            }
            current = current->next;
        }
        
        DrawSphereWires((Vector3){0, 3, 1.5}, 0.1, 10, 10, WHITE);
        
    End3dMode();
    
}

void play_alt_draw()
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

void play_ui_draw()
{
    console_draw();
    console_alt_draw();
}

void play_quit()
{
    cabinet_free(&arcade1);
    cabinet_free(&arcade2);
    cabinet_free(&arcade3);
}

void play_enter(gamestate_t* prev)
{
    
}

void play_leave(gamestate_t* next)
{
    
}

static gamestate_t play_state;
gamestate_t* play_getstate() {
    static bool i = false;
    if (!i) {
        play_state.initialized   = false;
        play_state.game_init     = &play_init;
        play_state.game_update   = &play_update;
        play_state.game_draw     = &play_draw;
        play_state.game_alt_draw = &play_alt_draw;
        play_state.game_ui_draw  = &play_ui_draw;
        play_state.game_quit     = &play_quit;
        play_state.game_leave    = &play_leave;
        play_state.game_enter    = &play_enter;
        i = true;
    }
    return &play_state;
}
