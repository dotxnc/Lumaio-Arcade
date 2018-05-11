#include "game.h"

#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include "resource.h"
#include "util/glad.h"
#include "util/gbuffer.h"
#include "util/raylibext.h"

static gbuffer_t gbuffer;
static Shader lighting;
static Shader ssao;
static Shader blur;

static RenderTexture2D ssao_buffer;
static RenderTexture2D blur_buffer;
static RenderTexture2D color_buffer;
static Matrix camera_matrix;

static unsigned int noise;

static float lerp(float a, float b, float f)
{
    return a + f * (b - a);
}

static float random()
{
    return (float)rand()/(float)RAND_MAX;
}


void game_init(game_t *game, int width, int height, void *title) {
    game->width = width;
    game->height = height;
    game->title = title;
    
    game->gamestate = NULL;
    
    InitWindow(width, height, title);
    SetMousePosition((Vector2){0, 0});
    SetTargetFPS(60);
    SetExitKey(KEY_F12);
    
    resource_loadmetadata("assets/metadata.txt");
    
    lighting = *resource_getshader("lighting");
    BindShaderTexture(lighting,    "colorbuffer", 1);
    BindShaderTexture(lighting,   "normalbuffer", 2);
    BindShaderTexture(lighting, "positionbuffer", 3);
    BindShaderTexture(lighting,     "ssaobuffer", 4);
    
    ssao = *resource_getshader("ssao");
    BindShaderTexture(ssao,   "normalbuffer", 1);
    BindShaderTexture(ssao,    "depthbuffer", 2);
    BindShaderTexture(ssao,    "noisebuffer", 3);
    BindShaderTexture(ssao, "positionbuffer", 4);
    
    blur = *resource_getshader("blur");
    BindShaderTexture(blur, "ssaobuffer", 1);
    
    SetShaderVector3(lighting, "lights[0].position", (Vector3){0, 3, 1.5});
    SetShaderVector3(lighting, "lights[0].color",    (Vector3){0, 1, 1});
    SetShaderFloat(lighting, "lights[0].linear",   0.9);
    SetShaderFloat(lighting, "lights[0].quadratic",0.32);
    
    SetShaderVector3(lighting, "lights[1].position", (Vector3){-3, 3, 0.5});
    SetShaderVector3(lighting, "lights[1].color",    (Vector3){0, 1, 1});
    SetShaderFloat(lighting, "lights[1].linear",   0.7);
    SetShaderFloat(lighting, "lights[1].quadratic",1.8);
    
    SetShaderVector3(lighting, "lights[2].position", (Vector3){3, 3, 1.5});
    SetShaderVector3(lighting, "lights[2].color",    (Vector3){0, 1, 1});
    SetShaderFloat(lighting, "lights[2].linear",   0.7);
    SetShaderFloat(lighting, "lights[2].quadratic",1.8);
    
    gbuffer = gbuffer_new(1280, 720);
    
    ssao_buffer = LoadRenderTexture(1280, 720);
    blur_buffer = LoadRenderTexture(1280, 720);
    color_buffer = LoadRenderTexture(1280, 720);
    
    // ssao samples and noise
    for (int i = 0; i < 64; i++) {
        Vector3 sample = {random() * 2.0 - 1.0, random() * 2.0 - 1.0, random()};
        sample = Vector3Normalize(sample);
        sample = Vector3Multiply(sample, random());
        float scale = (float)i/64.f;
        
        scale = lerp(0.1f, 1.f, scale*scale);
        sample = Vector3Multiply(sample, scale);
        
        SetShaderVector3(ssao, FormatText("samples[%d]", i), sample);
    }
    
    float ssaoNoise[16*3];
    for (int i = 0; i < 16*3; i+=3) {
        ssaoNoise[i+0] = (float)(rand()/(float)RAND_MAX)*2.0-1.0;
        ssaoNoise[i+1] = (float)(rand()/(float)RAND_MAX)*2.0-1.0;
        ssaoNoise[i+2] = 0.f;
    }
    
    glGenTextures(1, &noise);
    glBindTexture(GL_TEXTURE_2D, noise);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void game_setstate(game_t *game, gamestate_t *state) {
    if (!state->initialized) {
        state->game_ptr = game;
        state->game_init();
        state->initialized = true;
    }
    
    if (game->gamestate != NULL) {
        game->gamestate->game_leave(state);
        state->game_enter(game->gamestate);
    }
    game->gamestate = state;
}

void game_start(game_t *game) {
    Matrix p = MatrixPerspective(90.f*DEG2RAD, 1280.f/720.f, 0.01f, 1000.f);
    SetShaderValueMatrix(ssao, GetShaderUniform(ssao, "projection"), p);
    
    int camera_matrix_loc = GetShaderUniform(lighting, "camView");
    
    while (!WindowShouldClose()) {
        game->gamestate->game_update(GetFrameTime());
        
        BeginDrawing();
            ClearBackground(BLACK);
            
            game->gamestate->game_alt_draw();
            
            gbuffer_begin(&gbuffer);
                game->gamestate->game_draw();
            gbuffer_end();
            
            BeginTextureMode(ssao_buffer); BeginShaderMode(ssao);
                SetShaderTexture(gbuffer.normal, 1);
                SetShaderTexture(gbuffer.depth, 2);
                SetShaderTexturei(noise, 3);
                SetShaderTexture(gbuffer.position, 4);
                DrawTextureFlipped(color_buffer.texture);
            EndShaderMode(); EndTextureMode();
            
            BeginTextureMode(blur_buffer); BeginShaderMode(blur);
                SetShaderTexture(ssao_buffer.texture, 1);
                DrawTextureFlipped(color_buffer.texture);
            EndShaderMode(); EndTextureMode();
            
            BeginShaderMode(lighting);
                SetShaderValueMatrix(lighting, camera_matrix_loc, camera_matrix);
                SetShaderTexture(gbuffer.color, 1);
                SetShaderTexture(gbuffer.normal, 2);
                SetShaderTexture(gbuffer.position, 3);
                SetShaderTexture(blur_buffer.texture, 4);
                DrawTextureFlipped(color_buffer.texture);
            EndShaderMode();
            // DrawTextureFlipped(ssao_buffer.texture);
            DrawTexturePro(ssao_buffer.texture, (Rectangle){0, 0, 1280, -720}, (Rectangle){0, 0, 1280/4, 720/4}, Vector2Zero(), 0, WHITE);
            DrawTexturePro(gbuffer.normal, (Rectangle){0, 0, 1280, -720}, (Rectangle){0, 720/4, 1280/4, 720/4}, Vector2Zero(), 0, WHITE);
            DrawTexturePro(gbuffer.position, (Rectangle){0, 0, 1280, -720}, (Rectangle){0, 720/4*2, 1280/4, 720/4}, Vector2Zero(), 0, WHITE);
            DrawTexturePro(gbuffer.color, (Rectangle){0, 0, 1280, -720}, (Rectangle){0, 720/4*3, 1280/4, 720/4}, Vector2Zero(), 0, WHITE);
            
            game->gamestate->game_ui_draw();
        EndDrawing();
    }
}

void game_setcamera(Camera3D camera) {
    camera_matrix = GetCameraMatrix(camera);
}

void game_shutdown(game_t *game) { CloseWindow(); }
