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

int lua_Clear(lua_State* L)
{
    array_clear(&console_out);
    return 0;
}

int lua_Complete(lua_State* L)
{
    if (lua_istable(L, 1)) {
        array_clear(&console_complete);
        
        lua_pushvalue(L, -1);
        lua_pushnil(L);
        while (lua_next(L, -2)) {
            lua_pushvalue(L, -2);
            const char* value = lua_tostring(L, -2);
            array_push(&console_complete, (void*)value);
            lua_pop(L, 2);
        }
        lua_pop(L, 1);
    }
    return 0;
}

int lua_Reload(lua_State* L)
{
    const char* key = lua_tostring(L, 1);
    cabinet_t* ptr = hashmap_get(world_getarcades(), key);
    
    if (ptr != NULL) {
        lua_pushnil(L);
        cabinet_reload(ptr);
    }
    else
        lua_pushstring(L, "entity does not exist");
    
    return 1;
}

int lua_ConsoleReload(lua_State* L)
{
    console_reload();
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

static bool isa(char a) {
    static const char* b = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890-=!@#$%^&*()_+[]{}\\|/?.,<>~\"':; ";
    for (int i = 0; i < 94; i++) {
        if (b[i] == a) return true;
    }
    return false;
}

static void draw_rich_text(const char* text, int x, int y, int size)
{
    Color color = WHITE;
    int w = 0;
    char l[strlen(text)+1];
    memset(l, '\0', strlen(text)+1);
    
    for (int i = 0; i < strlen(text); i++)
    {
        char c = text[i];
        if (c == '{') {
            char hex[9] = "";
            for (int j = 0; j < 8; j++) {
                hex[j] = text[i+j+1];
            }
            hex[8] = '\0';
            color = GetColor(strtoul(hex, NULL, 16));
            i += 10;
            if (i > strlen(text)) break;
        } else {
            w++;
        }
        char t[2];
        sprintf(t, "%c", text[i]);
        if (w > 0 ) {
            int tw = MeasureText(l, size);
            DrawText(t, x+tw+1, y, size, color);
            // DrawTextEx(GetDefaultFont(), t, (Vector2){x+tw+1, y}, size, 1, color);
        } else {
            DrawText(t, x, y, size, color);
            // DrawTextEx(GetDefaultFont(), t, (Vector2){x, y}, size, 1, color);
        }
        l[strlen(l)] = text[i];
    }
}

void console_init()
{
    console_buffer = LoadRenderTexture(GetScreenWidth(), GetScreenWidth()/3);
    
    array_init(&console_out, 128+1);
    array_init(&console_complete, 128+1);
    
    memset(console_input, '\0', 128+1);
    console_input_length = 0;
    
    /// script
    console_L = luaL_newstate();
    luaL_openlibs(console_L);
    int status = 0;
    if ((status = luaL_loadfile(console_L, "assets/scripts/console.lua"))) {
        printf("Failed to init script (%s) (%d)\n", lua_tostring(console_L, -1), status);
    }
    
    lua_pushcfunction(console_L, lua_Conout);
    lua_setglobal(console_L, "conout");
    
    lua_pushcfunction(console_L, lua_Clear);
    lua_setglobal(console_L, "clear");
    
    lua_pushcfunction(console_L, lua_Complete);
    lua_setglobal(console_L, "_complete");
    
    lua_pushcfunction(console_L, lua_Reload);
    lua_setglobal(console_L, "_reload");
    
    lua_pushcfunction(console_L, lua_ConsoleReload);
    lua_setglobal(console_L, "console_reload");
    
    // prime script
    lua_pcall(console_L, 0, 0, NULL);
}

void console_print(const char* s)
{
    char o[128+1];
    strncpy(o, s, 128);
    o[128] = '\0';
    array_push(&console_out, o);
}

void console_reload()
{
    free(console_out.data);
    free(console_complete.data);
    
    if (console_L)
        lua_close(console_L);
    UnloadRenderTexture(console_buffer);
    console_init();
}

void console_update()
{
    if (!console_open) return;
    
    int c = GetKeyPressed();
    if (isa(c)) {
        if (console_input_length < 128) {
            console_input[console_input_length++] = c;
        }
        console_complete_index = -1;
        lua_getglobal(console_L, "_autocomplete");
        lua_pushstring(console_L, console_input);
        lua_pcall(console_L, 1, 0, 0);
    }
    if (IsKeyPressed(KEY_ENTER)) {
        int a = luaL_dostring(console_L, console_input);
        if (a) {
            console_print(FormatText("{ff0000ff}%s", lua_tostring(console_L, -1)));
        }
        console_complete_index = -1;
        memset(console_input, '\0', 128+1);
        console_input_length = 0;
        array_clear(&console_complete);
    }
    if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_GRAVE)) {
        console_complete_index = -1;
        memset(console_input, '\0', 128+1);
        console_input_length = 0;
        // console_toggle();
        array_clear(&console_complete);
    }
    if (IsKeyPressed(KEY_BACKSPACE)) {
        console_complete_index = -1;
        if (console_input_length > 0) {
            console_input[--console_input_length] = '\0';
            lua_getglobal(console_L, "_autocomplete");
            lua_pushstring(console_L, console_input);
            lua_pcall(console_L, 1, 0, 0);
            if (console_input_length == 0) {
                array_clear(&console_complete);
            }
        }
    }
    
    if (IsKeyPressed(KEY_TAB)) {
        if (console_complete.length > 0) {
            console_complete_index = (console_complete_index+1)%console_complete.length;
            strncpy(console_input, array_get(&console_complete, console_complete_index), 129);
            console_input_length = strlen(console_input);
        }
    }
}

void console_draw()
{
    if (!console_open) return;
    int w=console_buffer.texture.width, h=console_buffer.texture.height;
    BeginTextureMode(console_buffer);
        DrawRectangle(0, 0, w, h, (Color){0, 0, 0, 180});
        for (int i = 0; i < console_out.length; i++) {
            Vector2 s = MeasureTextEx(GetDefaultFont(), array_get(&console_out, i), 10, 1);
            draw_rich_text(array_get(&console_out, i), 10, h-s.y*(console_out.length-i), 10);
        }
    EndTextureMode();
    DrawLine(0, h, w, h, WHITE);
    DrawLine(0, h+30, w, h+30, WHITE);
    
    Rectangle sr = (Rectangle){0, 0, w, -h};
    Rectangle dr = (Rectangle){0, 0, w, h};
    DrawTexturePro(console_buffer.texture, sr, dr, (Vector2){0, 0}, 0, WHITE);
    
    DrawText(console_input, 10, h+10, 10, WHITE);
    int tw = MeasureText(console_input, 10);
    DrawLine(11+tw, h+10, 11+tw, h+20, WHITE);
    
    if (console_complete.length > 0) {
        for (int i = 0; i < console_complete.length; i++) {
            if (i == console_complete_index) {
                DrawRectangle(10, h+35+i*15, 250, 10, GRAY);
            }
            DrawText(array_get(&console_complete, i), 10, h+35+i*15, 10, WHITE);
        }
    }
}

void console_toggle()
{
    if (console_open){
        DisableCursor();
        SetMousePosition(mouse_fix);
    }
    else {
        EnableCursor();
        mouse_fix = GetMousePosition();
        SetMousePosition((Vector2){GetScreenWidth()/2,GetScreenHeight()/2});
    }
    console_open = !console_open;
}

bool console_isopen()
{
    return console_open;
}
