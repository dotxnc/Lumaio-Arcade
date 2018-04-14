#include "console.h"

/*********
Lua bindings to use in console functions
**********/

int lua_Conout(lua_State* L)
{
    const char* s = lua_tostring(L, 1);
    console_print(s);
    return 0;
}

/*********
End console lua bindings
**********/

bool console_get(const char* name)
{
    if (lua_getglobal(console_L, name)) {
        return true;
    }
    return false;
}

void console_pushstring(const char* str)
{
    lua_pushstring(console_L, str);
    console_num++;
}

void console_pushnumber(lua_Number n)
{
    lua_pushnumber(console_L, n);
    console_num++;
}

const char* console_call()
{
    if (lua_pcall(console_L, console_num, 0, NULL)) {
        console_print(lua_tostring(console_L, -1));
        return lua_tostring(console_L, -1);
    }
    console_num = 0;
    return NULL;
}

//_________________________________________________________________________________//

void console_init()
{
    console_buffer = LoadRenderTexture(GetScreenWidth(), GetScreenWidth()/3);
    
    /// script
    console_L = luaL_newstate();
    luaL_openlibs(console_L);
    int status = 0;
    if ((status = luaL_loadfile(console_L, "assets/scripts/console.lua"))) {
        printf("Failed to init script (%s) (%d)", lua_tostring(console_L, -1), status);
    }
    
    lua_pushcfunction(console_L, lua_Conout);
    lua_setglobal(console_L, "Conout");
    
    // prime script
    lua_pcall(console_L, 0, 0, NULL);
}

void console_print(const char* s)
{
    char o[128];
    strncpy(o, s, 128);
    sprintf(console_output, "%s%s\n", console_output, o);
}

void console_reload()
{
    if (console_L)
        lua_close(console_L);
    UnloadRenderTexture(console_buffer);
    console_init();
}

void console_update()
{
    if (!console_open) return;
}

void console_draw()
{
    if (!console_open) return;
    int w=console_buffer.texture.width, h=console_buffer.texture.height;
    BeginTextureMode(console_buffer);
        DrawRectangle(0, 0, w, h, (Color){0, 0, 0, 180});
        DrawRectangleLines(0, 0, w, h, WHITE);
    
        Vector2 s = MeasureTextEx(GetDefaultFont(), console_output, 10, 1);
        DrawText(console_output, 10, h-s.y, 10, WHITE);
    EndTextureMode();
    
    Rectangle sr = (Rectangle){0, 0, w, -h};
    Rectangle dr = (Rectangle){0, 0, w, h};
    DrawTexturePro(console_buffer.texture, sr, dr, (Vector2){0, 0}, 0, WHITE);
}

void console_toggle()
{
    console_open = !console_open;
}
