#include "playstate.h"
#include <raylib.h>
#include "resource.h"
#include "cabinet.h"
#include "console.h"

static cabinet_t arcade1;
static cabinet_t arcade2;
static cabinet_t arcade3;
static Camera camera;
static bool interacting = false;
static Vector2 save_mouse = (Vector2){0, 0};
static Vector3 light_position = (Vector3){1.5, 3, 2};

static Shader* lighting;
static int light_position_location;
static int light_viewpos_location;

void play_init()
{
    console_init();
    
    resource_loadmodel("assets/models/Arcade1.obj", "assets/textures/Arcade1_texture.png", "arcade1");
    resource_loadmodel("assets/models/Arcade1_screen.obj", NULL, "arcade1_screen");
    
    resource_loadmodel("assets/models/Arcade2.obj", "assets/textures/Arcade2_texture.png", "arcade2");
    resource_loadmodel("assets/models/Arcade2_screen.obj", NULL, "arcade2_screen");
    
    resource_loadshader("assets/shaders/light.vs", "assets/shaders/light.fs", "simple_lighting");
    resource_loadshader("assets/shaders/light.vs", "assets/shaders/vignette.fs", "vignette");
    
    lighting = resource_getshader("simple_lighting");
    light_position_location = GetShaderLocation(*lighting, "lightPos");
    light_viewpos_location = GetShaderLocation(*lighting, "viewPos");
    lighting->locs[LOC_MATRIX_MODEL] = GetShaderLocation(*lighting, "modelMatrix");
    resource_getmodel("arcade1")->material.shader = *lighting;
    resource_getmodel("arcade2")->material.shader = *lighting;
    
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
    
    hashmap_pushvalue(world_getarcades(), "test", &arcade1);
    hashmap_pushvalue(world_getarcades(), "snake", &arcade2);
    hashmap_pushvalue(world_getarcades(), "invaders", &arcade3);
    
    camera = (Camera){{0, 3.5, 5}, {0, 3, 0}, {0, 1, 0}, 90.f};
    SetCameraMode(camera, CAMERA_FIRST_PERSON);
}

void play_update(float dt)
{
    if (IsKeyPressed(KEY_GRAVE)) {
        console_toggle();
    }
    
    light_position.x = 1.5+cos(GetTime())*3;
    light_position.z = sin(GetTime())*3;
    
    SetShaderValue(*lighting, light_position_location, &light_position, 3);
    SetShaderValue(*lighting, light_viewpos_location, &camera.position, 3);
    
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
        DrawGrid(1000, 1);
        hashmap_t* current = world_getarcades();
        while (current != NULL) {
            cabinet_t* ptr = current->value;
            if (ptr != NULL) {
                cabinet_draw(ptr);
            }
            current = current->next;
        }
        
        DrawSphereWires(light_position, 0.1, 6, 6, WHITE);
        
        current = world_getarcades();
        while (current != NULL) {
            cabinet_t* ptr = current->value;
            if (ptr != NULL) {
                cabinet_drawgame(ptr);
            }
            current = current->next;
        }
        
    End3dMode();
    
    console_draw();
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
        play_state.initialized = false;
        play_state.game_init   = &play_init;
        play_state.game_update = &play_update;
        play_state.game_draw   = &play_draw;
        play_state.game_quit   = &play_quit;
        play_state.game_leave  = &play_leave;
        play_state.game_enter  = &play_enter;
        i = true;
    }
    return &play_state;
}
