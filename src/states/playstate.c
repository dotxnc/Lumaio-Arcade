#include "playstate.h"
#include <raylib.h>
#include "../resource.h"
#include "../cabinet.h"
#include "../console.h"
#include "../util/raylibext.h"
#include "../util/discordhelper.h"

static cabinet_t arcade1;
static cabinet_t arcade2;
static cabinet_t arcade3;
static cabinet_t arcade4;
static Camera camera = { 0 };
static bool interacting = false;
static Vector2 save_mouse = (Vector2){0, 0};
static Vector3 light_position = (Vector3){1.5, 3, 2};

static Shader* gbuffer;
static Shader* lighting;

void play_init()
{
    console_init();
    
    // TODO: move to resource manifest file???
    gbuffer = resource_getshader("gbuffer");
    gbuffer->locs[LOC_MATRIX_MODEL] = GetShaderLocation(*gbuffer, "modelMatrix");
    gbuffer->locs[LOC_MATRIX_VIEW] = GetShaderLocation(*gbuffer, "viewMatrix");
    gbuffer->locs[LOC_MATRIX_PROJECTION] = GetShaderLocation(*gbuffer, "projectionMatrix");
    gbuffer->locs[LOC_MAP_EMISSION] = GetShaderLocation(*gbuffer, "texture5");
    
    lighting = resource_getshader("lighting");
    
    // TODO: move to resource manifest file
    SetModelShader(resource_getmodel("arcade1"), *gbuffer);
    SetModelShader(resource_getmodel("arcade2"), *gbuffer);
    SetModelShader(resource_getmodel("arcade1_screen"), *gbuffer);
    SetModelShader(resource_getmodel("arcade2_screen"), *gbuffer);
    SetModelMap(resource_getmodel("arcade2"), MAP_EMISSION, *resource_gettexture("Arcade2_emission"));
    
    // initialize world from world file
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
    
    // TODO: move to console
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
    lua_pushnumber(cl, camera.fovy);
    lua_setfield(cl, -2, "fov");
    lua_setglobal(cl, "camera");
    
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
