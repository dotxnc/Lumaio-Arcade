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

int lua_CabinetSpawn(lua_State* L)
{
    const char* model = lua_tostring(L, 1);
    const char* script = lua_tostring(L, 2);
    const char* key = lua_tostring(L, 3);
    
    lua_settop(L, 4);
    lua_gettable(L, 1);
    
    lua_rawgeti(L, 4, 1);
    double x = lua_tonumber(L, -1);
    lua_rawgeti(L, 4, 2);
    double y = lua_tonumber(L, -1);
    lua_rawgeti(L, 4, 3);
    double z = lua_tonumber(L, -1);
    // printf("%.2f %.2f %.2f\n", x, y, z);
    printf("CABINET SPAWN: %s %s %s {%.2f %.2f %.2f}\n", model, script, key, x, y, z);
    if (resource_getmodel(model) == NULL) {
        console_print("{red}error: {white}can't find that model, aborting...");
        return 0;
    }
     
    cabinet_t* cabinet = malloc(sizeof(cabinet_t));
    cabinet_init(cabinet, model, script);
    cabinet->position = (Vector3){x, y, z};
    hashmap_pushvalue(world_getarcades(), key, cabinet);
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
    // behold my masterpiece
    Color color = WHITE;
    int w = 0;
    int h = 0;
    char l[strlen(text)+1];
    memset(l, '\0', strlen(text)+1);
    
    for (int i = 0; i < strlen(text); i++)
    {
        char c = text[i];
        if (c == '{') {
            bool is_color = true;
            int b = i+1;
            char unhashed_color[32];
            int d = 0;
            memset(unhashed_color, '\0', 32);
            while (b < strlen(text) && text[b] != '}' && d < 32) {
                unhashed_color[d++] = text[b++];
            }
            unsigned int hash = xcrc32(&unhashed_color, strlen(unhashed_color), 0xffffffff);
            switch (hash) {
                case 2904053514: { // xcrc32(red, 3, 0xffffffff);
                    color = (Color){255, 0, 0, 255};
                } break;
                case 3839832734: { // xcrc32(green, 5, 0xffffffff);
                    color = (Color){0, 255, 0, 255};
                } break;
                case 1502922994: { // xcrc32(blue, 4, 0xffffffff);
                    color = (Color){0, 0, 255, 255};
                } break;
                case 1110385510: { // xcrc32(yellow, 6, 0xffffffff);
                    color = (Color){255, 255, 0, 255};
                } break;
                case 3774346613: { // xcrc32(cyan, 4, 0xffffffff);
                    color = (Color){0, 255, 255, 255};
                } break;
                case 4006689598: { // xcrc32(purple, 6, 0xffffffff);
                    color = (Color){255, 0, 255, 255};
                } break;
                case 894297704: { // xcrc32(white, 5, 0xffffffff);
                    color = (Color){255, 255, 255, 255};
                } break;
                case 3058065716: { // xcrc32(black, 5, 0xffffffff);
                    color = (Color){0, 0, 0, 255};
                } break;
                default: {
                    if (unhashed_color[0] == '0' && unhashed_color[1] == 'x') {
                        if (strlen(unhashed_color) < 10) {
                            unhashed_color[8] = 'f';
                            unhashed_color[9] = 'f';
                        }
                        color = GetColor(strtoul(unhashed_color, NULL, 0));
                    }else {
                        // if (strlen(unhashed_color) < 10) {
                        //     unhashed_color[6] = 'f';
                        //     unhashed_color[7] = 'f';
                        // }
                        // color = GetColor(strtoul(unhashed_color, NULL, 16));
                        is_color = false;
                    }
                } break;
            }
            if (is_color) {
                i = b+1;
                c = text[i];
            }
        }
        if (c == '\n') {
            memset(l, '\0', strlen(text)+1);
            h++;
            w=0;
        }
        if (c != '\n' && c != '{') w++;
        char t[2];
        sprintf(t, "%c", text[i]);
        if (w > 0 ) {
            int tw = MeasureText(l, size);
            DrawText(t, x+tw+1, y+size*h+1, size, color);
        } else {
            DrawText(t, x, y+size*h+1, size, color);
        }
        if (c != '\n') {
            l[strlen(l)] = text[i];
        }
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
    
    lua_pushcfunction(console_L, lua_CabinetSpawn);
    lua_setglobal(console_L, "cabinet_spawn");
    
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
    if (IsKeyPressed(KEY_ENTER) && console_input_length > 0) {
        console_print(FormatText("{0xaaaaaaff}> %s", console_input));
        strcpy(console_last, console_input);
        int a = luaL_dostring(console_L, console_input);
        if (a) {
            console_print(FormatText("{0xff0000ff}%s", lua_tostring(console_L, -1)));
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
    
    if (console_complete.length > 0) {
        if (IsKeyPressed(KEY_TAB) || IsKeyPressed(KEY_DOWN)) {
            console_complete_index = (console_complete_index+1)%console_complete.length;
            strncpy(console_input, array_get(&console_complete, console_complete_index), 129);
            console_input_length = strlen(console_input);
        }
        if (IsKeyPressed(KEY_UP)) {
            console_complete_index--;
            if (console_complete_index < 0)
                console_complete_index = console_complete.length-1;
            strncpy(console_input, array_get(&console_complete, console_complete_index), 129);
            console_input_length = strlen(console_input);
        }
    } else {
        if (IsKeyPressed(KEY_UP) && strlen(console_last) > 0) {
            strcpy(console_input, console_last);
            console_input_length = strlen(console_input);
        }
    }
}

void console_draw()
{
    if (!console_open) return;
    int w=console_buffer.texture.width, h=console_buffer.texture.height;
    DrawRectangle(0, 0, w, h+30, (Color){5, 2, 2, 240});
    
    BeginTextureMode(console_buffer);
        ClearBackground((Color){0, 0, 0, 0});
        for (int i = 0; i < console_out.length; i++) {
            Vector2 s = MeasureTextEx(GetDefaultFont(), array_get(&console_out, i), 10, 1);
            draw_rich_text(array_get(&console_out, i), 10, h-s.y*(console_out.length-i)-10, 10);
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
        int h2 = h+30+console_complete.length*15;
        DrawRectangle(0, h+31, 270-1, h2-h-31, (Color){5, 2, 2, 240});
        for (int i = 0; i < console_complete.length; i++) {
            if (i == console_complete_index) {
                DrawRectangle(10, h+35+i*15, 250, 10, GRAY);
            }
            DrawText(array_get(&console_complete, i), 10, h+35+i*15, 10, WHITE);
        }
        DrawLine(0, h2, 270, h2, WHITE);
        DrawLine(270, h+30, 270, h2, WHITE);
    }
    
    draw_rich_text("{red}This is some\n{green}rich text stuff\n{0xaa0000}and more!\n{0xffaaff}I might be slighting autistic", 50, 50, 10);
}

void console_alt_draw()
{
    
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

lua_State* console_getstate()
{
    return console_L;
}
