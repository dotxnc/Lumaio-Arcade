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

static void camera_update(Camera3D* c, float speed)
{
    printf("FUCK");
    DisableCursor();
    
    static Vector3 camera_angle = {0.f};
    
    static Vector2 mouse_prev = {0.f};
    Vector2 mouse_delta = {0.f};
    Vector2 mouse_now = GetMousePosition();
    
    mouse_delta.x = mouse_now.x - mouse_prev.x;
    mouse_delta.y = mouse_now.y - mouse_prev.y;
    mouse_prev = mouse_now;
    
    camera_angle.x += mouse_delta.x*0.003;
    camera_angle.y += -mouse_delta.y*0.003;
    camera_angle.y = Clamp(camera_angle.y, -89.f*DEG2RAD, 89.f*DEG2RAD);
    printf("%.2f\r", camera_angle.y);
    
    Vector3 front;
    front.x = cos(camera_angle.y) * cos(camera_angle.x);
    front.y = sin(camera_angle.y);
    front.z = cos(camera_angle.y) * sin(camera_angle.x);
    Vector3 cf = Vector3Normalize(front);
    
    if (IsKeyDown(KEY_W)) {
        float dx = cosf(camera_angle.x);
        float dy = sinf(camera_angle.x);
        c->position.x += dx*(speed*GetFrameTime());
        c->position.z += dy*(speed*GetFrameTime());
        // camera.position = Vector3Add(camera.position, Vector3Multiply(cf, speed*GetFrameTime()));
    }
    if (IsKeyDown(KEY_S)) {
        float dx = cosf(camera_angle.x);
        float dy = sinf(camera_angle.x);
        c->position.x -= dx*(speed*GetFrameTime());
        c->position.z -= dy*(speed*GetFrameTime());
        // camera.position = Vector3Subtract(camera.position, Vector3Multiply(cf, speed*GetFrameTime()));
    }
    if (IsKeyDown(KEY_A)) {
        c->position = Vector3Subtract(c->position, Vector3Multiply(Vector3Normalize(Vector3CrossProduct(cf, c->up)), speed*GetFrameTime()));
    }
    if (IsKeyDown(KEY_D)) {
        c->position = Vector3Add(c->position, Vector3Multiply(Vector3Normalize(Vector3CrossProduct(cf, c->up)), speed*GetFrameTime()));
    }
    
    c->target = Vector3Add(c->position, cf);
}

void play_init()
{
    lighting = resource_getshader("lighting");
    console_init();
    for (int i = 0; i < 1; i++) {
        world_free();
        world_initialize("assets/maps/test.txt");
    }
    
    // Camera camera = { 0 };
    camera.position = (Vector3){ 0.f, 3.5f, 1.f };
    camera.target = (Vector3){ 0.0f, 1.8f, 0.0f };
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 90.0f;
    camera.type = CAMERA_PERSPECTIVE;
    SetCameraMode(camera, CAMERA_FREE);
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
        camera_update(&camera, 3);
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
