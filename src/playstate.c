#include "playstate.h"
#include <raylib.h>
#include "resource.h"
#include "cabinet.h"

static cabinet_t arcade1;
static cabinet_t arcade2;
static Camera camera;
static bool interacting = false;
static Vector2 save_mouse = (Vector2){0, 0};
static Vector3 light_position = (Vector3){1.5, 3, 2};

static Shader* lighting;
static int light_position_location;
static int light_viewpos_location;

void play_init()
{
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
    
    cabinet_rotate(&arcade1, 0, -0.3, 0);
    cabinet_rotate(&arcade2, 0, 0.3, 0);
    
    cabinet_setshader(&arcade2, "vignette");
    
    arcade2.position = (Vector3){3, 0, -0.75};
    
    camera = (Camera){{0, 3.5, 5}, {0, 3, 0}, {0, 1, 0}, 90.f};
    SetCameraMode(camera, CAMERA_FIRST_PERSON);
}

void play_update(float dt)
{
    if (IsKeyPressed(KEY_E)) {
        // check if looking at cabinet
        Ray r;
        r.position = camera.position;
        r.direction = Vector3Subtract(camera.target, camera.position);
        
        RayHitInfo h = GetCollisionRayModel(r, arcade1.screen);
        RayHitInfo h2 = GetCollisionRayModel(r, arcade2.screen);
        if (h.hit) {
            interacting = true;
            arcade1.interacting = true;
        }
        if (h2.hit) {
            interacting = true;
            arcade2.interacting = true;
        }
    }
    
    if (!interacting) {
        UpdateCamera(&camera);
        save_mouse = GetMousePosition();
    } else {
        if (IsKeyPressed(KEY_ESCAPE)) {
            interacting = false;
            arcade1.interacting = false;
            arcade2.interacting = false;
            SetMousePosition(save_mouse);
        }
    }
    
    light_position.x = 1.5+cos(GetTime())*3;
    light_position.z = sin(GetTime())*3;
    
    SetShaderValue(*lighting, light_position_location, &light_position, 3);
    SetShaderValue(*lighting, light_viewpos_location, &camera.position, 3);
    
    cabinet_update(&arcade1);
    cabinet_update(&arcade2);
    
}

void play_draw()
{
    Ray r;
    r.position = camera.position;
    r.direction = Vector3Subtract(camera.target, camera.position);
    RayHitInfo h1 = GetCollisionRayModel(r, arcade1.screen);
    RayHitInfo h2 = GetCollisionRayModel(r, arcade2.screen);
    
    Begin3dMode(camera);
        // DrawGrid(100, 1);
        
        cabinet_draw(&arcade1);
        cabinet_draw(&arcade2);
        
        DrawSphereWires(light_position, 0.1, 6, 6, WHITE);
        
        cabinet_drawgame(&arcade1);
        cabinet_drawgame(&arcade2);
        
    End3dMode();
    
    if (h1.hit || h2.hit) {
        DrawText("HIT", 10, 10, 20, RED);
    }
}

void play_quit()
{
    cabinet_free(&arcade1);
    cabinet_free(&arcade2);
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
