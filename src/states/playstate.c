#include "playstate.h"
#include <raylib.h>
#include "../resource.h"
#include "../cabinet.h"
#include "../console.h"
#include "../util/raylibext.h"
#include "../util/discordhelper.h"

static Camera camera = { 0 };
static bool interacting = false;
static Vector2 save_mouse = (Vector2){0, 0};

static Shader* gbuffer;
static Shader* lighting;

void play_init()
{
    lighting = resource_getshader("lighting");
    console_init();
    world_initialize("assets/maps/test.txt");
    
    // Camera camera = { 0 };
    camera.position = (Vector3){ 0.f, 3.5f, 1.f };
    camera.target = (Vector3){ 0.0f, 1.8f, 0.0f };
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 90.0f;
    camera.type = CAMERA_PERSPECTIVE;
    SetCameraMode(camera, CAMERA_FIRST_PERSON);
}

void play_update(float dt)
{
    
    console_pushcamera(camera);
    
    SetShaderVector3(*lighting, "viewpos", camera.position);
    
    if (IsKeyPressed(KEY_GRAVE)) {
        console_toggle();
    }
    
    world_update(dt, camera, &interacting);
    
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
    game_setcamera(camera);
    
}

void play_draw()
{
    BeginMode3D(camera);
        world_draw();
    EndMode3D();
    
}

void play_alt_draw()
{
    world_alt_draw();
}

void play_ui_draw()
{
    console_draw();
    console_alt_draw();
}

void play_quit()
{
    world_free();
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
