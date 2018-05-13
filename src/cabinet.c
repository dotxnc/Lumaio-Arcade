#include "cabinet.h"

/*********
These are the lua bindings to raylib draw functions
**********/

static Color global_color = WHITE;

static bool is_interacting(lua_State* L)
{
    int i = false;
    lua_getglobal(L, "arcade");
    lua_getfield(L, -1, "interacting");
    i = lua_toboolean(L, -1);
    return i;
}

static int lua_SetColor(lua_State* L)
{
    global_color.r = lua_tonumber(L, 1);
    global_color.g = lua_tonumber(L, 2);
    global_color.b = lua_tonumber(L, 3);
    global_color.a = 255;
    if (!lua_isnil(L, 4)) global_color.a = lua_tonumber(L, 4);
    return 0;
}

static int lua_DrawText(lua_State* L)
{
    const char* text = lua_tostring(L, 1);
    double x = lua_tonumber(L, 2);
    double y = lua_tonumber(L, 3);
    double s = lua_tonumber(L, 4);
    DrawText(text, x, y, (int)s, global_color);
    return 0;
}

static int lua_GetTextWidth(lua_State* L)
{
    const char* text = lua_tostring(L, 1);
    double s = lua_tonumber(L, 2);
    lua_pushnumber(L, MeasureText(text, (int)s));
    return 1;
}

static int lua_GetTime(lua_State* L)
{
    lua_pushnumber(L, GetTime());
    return 1;
}

static int lua_IsKeyPressed(lua_State* L)
{
    int k = (int)lua_tonumber(L, 1);
    lua_pushboolean(L, is_interacting(L) ? IsKeyPressed(k) : 0);
    return 1;
}

static int lua_IsKeyDown(lua_State* L)
{
    int k = (int)lua_tonumber(L, 1);
    lua_pushboolean(L, is_interacting(L) ? IsKeyDown(k) : 0);
    return 1;
}

static int lua_DrawRectangle(lua_State* L)
{
    const char* type = lua_tostring(L, 1); // "fill", "line"
    double x = lua_tonumber(L, 2);
    double y = lua_tonumber(L, 3);
    double w = lua_tonumber(L, 4);
    double h = lua_tonumber(L, 5);
    if (strncmp(type, "line", 4))
        DrawRectangle(x, y, w, h, global_color);
    else if (strncmp(type, "fill", 4))
        DrawRectangleLines(x, y, w, h, global_color);
    return 0;
}

static int lua_DrawCircle(lua_State* L)
{
    const char* type = lua_tostring(L, 1); // "fill", "line"
    double x = lua_tonumber(L, 2);
    double y = lua_tonumber(L, 3);
    double r = lua_tonumber(L, 4);
    if (strncmp(type, "line", 4))
        DrawCircle(x, y, r, global_color);
    else if (strncmp(type, "fill", 4))
        DrawCircleLines(x, y, r, global_color);
    return 0;
}

static int lua_DrawLine(lua_State* L)
{
    double x1 = lua_tonumber(L, 1);
    double y1 = lua_tonumber(L, 2);
    double x2 = lua_tonumber(L, 3);
    double y2 = lua_tonumber(L, 4);
    DrawLine(x1, y1, x2, y2, global_color);
    return 0;
}

static int lua_DrawPixel(lua_State* L)
{
    double x = lua_tonumber(L, 1);
    double y = lua_tonumber(L, 2);
    DrawPixel(x, y, global_color);
    return 0;
}

static int lua_ClearColor(lua_State* L)
{
    double r = lua_tonumber(L, 1);
    double g = lua_tonumber(L, 2);
    double b = lua_tonumber(L, 3);
    double a = 255;
    if (!lua_isnil(L, 4)) a = lua_tonumber(L, 4);
    ClearBackground((Color){r,g,b,a});
    return 0;
}

static int lua_LoadTexture(lua_State* L)
{
    const char* file = lua_tostring(L, 1);
    const char* key = lua_tostring(L, 2);
    resource_loadtexture(file, key);
    return 0;
}

static int lua_DrawTexture(lua_State* L)
{
    const char* key = lua_tostring(L, 1);
    double x = lua_tonumber(L, 2);
    double y = lua_tonumber(L, 3);
    double scale = 1;
    if (!lua_isnil(L, 4)) scale = lua_tonumber(L, 4);
    Texture2D* ptr = resource_gettexture(key);
    if (ptr == NULL) {
        luaL_error(L, "texture does not exist");
    } else {
        DrawTextureEx(*ptr, (Vector2){x, y}, 0, scale, global_color);
    }
    return 0;
}

/*********
End raylib lua bindings
**********/

static void cabinet_loadscript(cabinet_t* cabinet)
{
    // initialize script
    int status = 0;
    cabinet->L = luaL_newstate();
    luaL_openlibs(cabinet->L);
    status = luaL_loadfile(cabinet->L, cabinet->script_file);
    if (status) {
        console_print(FormatText("{0xff0000}Failed to init script (%s) (%d)", lua_tostring(cabinet->L, -1), status));
        luaL_loadstring(cabinet->L, NOSCRIPT);
    }
    
    // arcade table
    lua_newtable(cabinet->L);
    
    // functions
    lua_pushcfunction(cabinet->L, &lua_DrawText);
    lua_setfield(cabinet->L, -2, "DrawText");
    
    lua_pushcfunction(cabinet->L, &lua_SetColor);
    lua_setfield(cabinet->L, -2, "SetColor");
    
    lua_pushcfunction(cabinet->L, &lua_GetTextWidth);
    lua_setfield(cabinet->L, -2, "GetTextWidth");
    
    lua_pushcfunction(cabinet->L, &lua_GetTime);
    lua_setfield(cabinet->L, -2, "GetTime");
    
    lua_pushcfunction(cabinet->L, &lua_IsKeyPressed);
    lua_setfield(cabinet->L, -2, "IsKeyPressed");
    
    lua_pushcfunction(cabinet->L, &lua_IsKeyDown);
    lua_setfield(cabinet->L, -2, "IsKeyDown");
    
    lua_pushcfunction(cabinet->L, &lua_DrawRectangle);
    lua_setfield(cabinet->L, -2, "DrawRectangle");
    
    lua_pushcfunction(cabinet->L, &lua_DrawCircle);
    lua_setfield(cabinet->L, -2, "DrawCircle");
    
    lua_pushcfunction(cabinet->L, &lua_DrawLine);
    lua_setfield(cabinet->L, -2, "DrawLine");
    
    lua_pushcfunction(cabinet->L, &lua_DrawPixel);
    lua_setfield(cabinet->L, -2, "DrawPixel");
    
    lua_pushcfunction(cabinet->L, &lua_ClearColor);
    lua_setfield(cabinet->L, -2, "ClearColor");
    
    lua_pushcfunction(cabinet->L, &lua_LoadTexture);
    lua_setfield(cabinet->L, -2, "LoadTexture");
    
    lua_pushcfunction(cabinet->L, &lua_DrawTexture);
    lua_setfield(cabinet->L, -2, "DrawTexture");
    
    // keys
    lua_pushnumber(cabinet->L, KEY_UP);
    lua_setfield(cabinet->L, -2, "KEY_UP");
    
    lua_pushnumber(cabinet->L, KEY_DOWN);
    lua_setfield(cabinet->L, -2, "KEY_DOWN");
    
    lua_pushnumber(cabinet->L, KEY_LEFT);
    lua_setfield(cabinet->L, -2, "KEY_LEFT");
    
    lua_pushnumber(cabinet->L, KEY_RIGHT);
    lua_setfield(cabinet->L, -2, "KEY_RIGHT");
    
    lua_pushnumber(cabinet->L, KEY_Z);
    lua_setfield(cabinet->L, -2, "KEY_Z");
    
    lua_pushnumber(cabinet->L, KEY_X);
    lua_setfield(cabinet->L, -2, "KEY_X");
    
    // other
    lua_pushboolean(cabinet->L, cabinet->interacting);
    lua_setfield(cabinet->L, -2, "interacting");
    
    lua_pushnumber(cabinet->L, 512);
    lua_setfield(cabinet->L, -2, "width");
    
    lua_pushnumber(cabinet->L, 512);
    lua_setfield(cabinet->L, -2, "height");
    
    // arcade global
    lua_setglobal(cabinet->L, "arcade");
    
    lua_pushcfunction(cabinet->L, &lua_Conout);
    lua_setglobal(cabinet->L, "conout");
    
    // prime script
    lua_pcall(cabinet->L, 0, 0, NULL);
}

void cabinet_init(cabinet_t* cabinet, const char* model_name, const char* script_name)
{
    // char screen_name[64];
    // sprintf(screen_name, "%s_screen", model_name);
    const char* screen_name = FormatText("%s_screen", model_name);
    
    cabinet->shader = NULL;
    
    cabinet->interacting = false;
    
    cabinet->machine = resource_getmodel(model_name);
    cabinet->screen = resource_getmodel(screen_name);
    
    cabinet->position = (Vector3){0, 0, 0};
    cabinet->rotation = MatrixIdentity();
    
    cabinet->target = LoadRenderTexture(512, 512);
    cabinet->temp = LoadRenderTexture(512, 512);
    SetTextureFilter(cabinet->target.texture, FILTER_POINT);
    SetTextureFilter(cabinet->temp.texture, FILTER_POINT);
    
    cabinet->L = NULL;
    strcpy(cabinet->script_file, script_name);
    
    cabinet_reload(cabinet);
    
}

void cabinet_reload(cabinet_t* cabinet)
{
    cabinet->errored = false;
    
    if (cabinet->L != NULL) {
        lua_close(cabinet->L);
        cabinet->L = NULL;
    }
    cabinet_loadscript(cabinet);
    
    lua_getglobal(cabinet->L, "init");
    lua_pcall(cabinet->L, 0, 0, NULL);
    
    
    
}

void cabinet_rotate(cabinet_t* cabinet, float x, float y, float z)
{
    Matrix r = MatrixIdentity();
    r = MatrixMultiply(r, MatrixRotateX(x));
    r = MatrixMultiply(r, MatrixRotateY(y));
    r = MatrixMultiply(r, MatrixRotateZ(z));
    cabinet->rotation = r;
}

void cabinet_setshader(cabinet_t* cabinet, const char* name)
{
    cabinet->shader = resource_getshader(name);
}

void cabinet_update(cabinet_t* cabinet)
{
    if (IsKeyPressed(KEY_ESCAPE)) {
        cabinet->interacting = false;
        discordhelper_setarcade(NULL, 0);
    }
    
    if (!cabinet->errored) {
        lua_getglobal(cabinet->L, "arcade");
        lua_pushboolean(cabinet->L, cabinet->interacting);
        lua_setfield(cabinet->L, -2, "interacting");
        
        int f = lua_getglobal(cabinet->L, "update");
        if (f > 0) {
            lua_pushnumber(cabinet->L, GetFrameTime());
            int s = lua_pcall(cabinet->L, 1, 0, NULL);
            if (s > 0) {
                console_print(FormatText("LUA ERROR: %s", lua_tostring(cabinet->L, -1)));
                cabinet->errored = true;
            }
        }
    }
    
    Matrix m = MatrixIdentity();
    m = MatrixMultiply(m, MatrixTranslate(cabinet->position.x, cabinet->position.y, cabinet->position.z));
    m = MatrixMultiply(m, cabinet->rotation);
    cabinet->machine->transform = m;
    cabinet->screen->transform = m;
}

void cabinet_draw(cabinet_t* cabinet)
{
    cabinet->screen->material.maps[MAP_DIFFUSE].texture = cabinet->target.texture;//cabinet->target.texture;
    cabinet->screen->material.maps[MAP_EMISSION].texture = cabinet->target.texture;
    
    Matrix m = MatrixIdentity();
    m = MatrixMultiply(m, MatrixTranslate(cabinet->position.x, cabinet->position.y, cabinet->position.z));
    m = MatrixMultiply(m, cabinet->rotation);
    cabinet->machine->transform = m;
    cabinet->screen->transform = m;
    DrawModel(*cabinet->machine, (Vector3){0, 0, 0}, 1.f, WHITE);
    DrawModel(*cabinet->screen, (Vector3){0, 0, 0}, 1.f, WHITE);
    
}

void cabinet_drawgame(cabinet_t* cabinet)
{
    BeginTextureMode(cabinet->temp);
        if (!cabinet->errored) {
            int f = lua_getglobal(cabinet->L, "draw");
            if (f > 0) {
                int s = lua_pcall(cabinet->L, 0, 0, NULL);
                if (s > 0) {
                    console_print(FormatText("LUA ERROR: %s", lua_tostring(cabinet->L, -1)));
                    cabinet->errored = true;
                }
            }
        } else {
            DrawText("ERROR: CHECK CONSOLE", 256-MeasureText("ERROR: CHECK CONSOLE", 30)/2, 226, 30, (Color){255,0,0,255});
        }
    EndTextureMode();
    
    BeginTextureMode(cabinet->target);
        static Rectangle sr = (Rectangle){0, 0, 512, 512};
        static Rectangle dr = (Rectangle){0, 0, 512, 512};
        
        if (cabinet->shader != NULL) {
            BeginShaderMode(*(cabinet->shader));
                DrawTexturePro(cabinet->temp.texture, sr, dr, (Vector2){0, 0}, 0, WHITE);
            EndShaderMode();
        } else {
            DrawTexturePro(cabinet->temp.texture, sr, dr, (Vector2){0, 0}, 0, WHITE);
        }
    EndTextureMode();
}

void cabinet_free(cabinet_t* cabinet)
{
    if (cabinet->L != NULL)
        lua_close(cabinet->L);
}
