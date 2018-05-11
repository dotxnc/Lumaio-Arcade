#include "editorstate.h"
#include <raylib.h>
#include "../resource.h"
#include "../cabinet.h"
#include "../console.h"
#include "../world.h"
#include "../util/array.h"

static Camera3D camera;
static array_t cubes;

static void camera_free()
{
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
    
    if (IsKeyDown(KEY_E)) {
        camera.position.y += 20*GetFrameTime();
    }
    if (IsKeyDown(KEY_Q)) {
        camera.position.y -= 20*GetFrameTime();
    }
    if (IsKeyDown(KEY_W)) {
        camera.position = Vector3Add(camera.position, Vector3Multiply(cf, 20*GetFrameTime()));
    }
    if (IsKeyDown(KEY_S)) {
        camera.position = Vector3Subtract(camera.position, Vector3Multiply(cf, 20*GetFrameTime()));
    }
    if (IsKeyDown(KEY_A)) {
        camera.position = Vector3Subtract(camera.position, Vector3Multiply(Vector3Normalize(Vector3CrossProduct(cf, camera.up)), 20*GetFrameTime()));
    }
    if (IsKeyDown(KEY_D)) {
        camera.position = Vector3Add(camera.position, Vector3Multiply(Vector3Normalize(Vector3CrossProduct(cf, camera.up)), 20*GetFrameTime()));
    }
    
    camera.target = Vector3Add(camera.position, cf);
    
}

void editor_init()
{
    camera = (Camera){{0, 3, 0}, {0, 0, 0}, {0, 1, 0}, 90.f};
    // SetCameraMode(camera, CAMERA_FREE);
}

Vector2 mouse_old;
void editor_update(float dt)
{
    if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON))
        camera_free();
    else
        EnableCursor();
    
}

void editor_draw()
{
    BeginMode3D(camera);
        DrawGrid(100, 1);
    EndMode3D();
    
    DrawText("Editor state", 10, 10, 40, GREEN);
}

void editor_alt_draw()
{
    
}

void editor_ui_draw()
{
    
}

void editor_quit()
{
    
}

void editor_enter(gamestate_t* prev)
{
    
}

void editor_leave(gamestate_t* enter)
{
    
}

gamestate_t* editor_getstate()
{
    static gamestate_t editor_state;
    static bool i = false;
    if (!i) {
        editor_state.initialized = false;
        editor_state.game_init   = &editor_init;
        editor_state.game_update = &editor_update;
        editor_state.game_draw   = &editor_draw;
        editor_state.game_alt_draw   = &editor_alt_draw;
        editor_state.game_ui_draw   = &editor_ui_draw;
        editor_state.game_quit   = &editor_quit;
        editor_state.game_leave  = &editor_leave;
        editor_state.game_enter  = &editor_enter;
        i = true;
    }
    return &editor_state;
}

