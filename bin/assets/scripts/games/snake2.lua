
local DrawText = arcade.DrawText
local DrawRectangle = arcade.DrawRectangle
local LoadTexture = arcade.LoadTexture
local DrawTexture = arcade.DrawTexture
local SetColor = arcade.SetColor
local GetTextWidth = arcade.GetTextWidth
local IsKeyPressed = arcade.IsKeyPressed

local KEY_UP = arcade.KEY_UP
local KEY_DOWN = arcade.KEY_DOWN
local KEY_LEFT = arcade.KEY_LEFT
local KEY_RIGHT = arcade.KEY_RIGHT
local KEY_Z = arcade.KEY_Z
local KEY_X = arcade.KEY_X

getmetatable('').__index = function(str,i) return string.sub(str,i,i) end

local map = {}
local width, height = 16, 16
local center = (16*20)/2
local moving = "east"
local snake = {}
local timer = 0
local need_tail = false
local dead = false
local menu = false
local score = 0

function deepcopy(orig)
    local orig_type = type(orig)
    local copy
    if orig_type == 'table' then
        copy = {}
        for orig_key, orig_value in next, orig, nil do
            copy[deepcopy(orig_key)] = deepcopy(orig_value)
        end
        setmetatable(copy, deepcopy(getmetatable(orig)))
    else -- number, string, boolean, etc
        copy = orig
    end
    return copy
end

function init()
    conout("{0x00ffff}[SNAKE V2] {white}snake v2 initializing")
    map = {}
    snake = {}
    
    for i=1,width*height do
        local x = i/width
        local y = i%width
        map[i] = {type="none"}
    end
    for i=1,3 do
        table.insert(snake, {x=8-i, y=8, move={x=1,y=0}})
    end
    spawn_food()
    
    dead = false
    need_tail = false
    timer = 0
    menu = true
    score = 0
end

function snake_update()
    if need_tail then
        table.insert(snake, {x=snake[#snake].x-snake[#snake].move.x, y=snake[#snake].y-snake[#snake].move.y, move={x=snake[#snake].move.x, y=snake[#snake].move.y}})
        need_tail = false
    end
    for i,v in ipairs(snake) do
        v.x = v.x + v.move.x
        v.y = v.y + v.move.y
    end
    local s_clone = deepcopy(snake)
    for i=2,#snake do
        local s = s_clone[i]
        s.move.x = snake[i-1].move.x
        s.move.y = snake[i-1].move.y
    end
    snake = deepcopy(s_clone)
    
    -- dead checks
    if snake[1].x > width-1 or snake[1].x < 0 or snake[1].y > height-1 or snake[1].y < 0 then
        dead = true
    end
    for i=2,#snake do
        if snake[1].x == snake[i].x and snake[1].y == snake[i].y then
            dead = true
            break
        end
    end
end

function spawn_food()
    local free = {}
    for i=1,width*height do
        local j = i-1
        local x = math.floor(j/width)
        local y = j%width
        local f = true
        for _,v in ipairs(snake) do
            if x == v.x and y == v.y then
                f = false
            end
        end
        if f then
            table.insert(free, i)
        end
    end
    map[free[math.random(#free)]].type = "food"
end

function update(dt)
    if menu then update_menu(dt) end
    if not menu then update_game(dt) end
end

function update_menu(dt)
    if IsKeyPressed(KEY_Z) then
        menu = false
    end
end

function update_game(dt)
    if dead then
        if IsKeyPressed(KEY_Z) then
            init()
        end
        return
    end
    
    if not arcade.interacting then return end
    
    timer = timer + dt
    
    if IsKeyPressed(KEY_UP) and snake[1].move.y == 0 then
        snake[1].move.x = 0
        snake[1].move.y = -1
        snake_update()
        timer = 0
    end
    if IsKeyPressed(KEY_DOWN) and snake[1].move.y == 0 then
        snake[1].move.x = 0
        snake[1].move.y = 1
        snake_update()
        timer = 0
    end
    if IsKeyPressed(KEY_LEFT) and snake[1].move.x == 0 then
        snake[1].move.x = -1
        snake[1].move.y = 0
        snake_update()
        timer = 0
    end
    if IsKeyPressed(KEY_RIGHT) and snake[1].move.x == 0 then
        snake[1].move.x = 1
        snake[1].move.y = 0
        snake_update()
        timer = 0
    end
    
    if timer > 0.1 then
        snake_update()
        timer = 0
    end
    
    for i=1,width*height do
        local j = i-1
        local x = math.floor(j/width)
        local y = j%width
        if map[i].type == "food" and x == snake[1].x and y == snake[1].y then
            need_tail = true
            map[i].type = "none"
            spawn_food()
            score = score + 1
        end
    end
end


function draw()
    SetColor(25, 25, 25, 255)
    DrawRectangle("fill", 0, 0, arcade.width, arcade.height)
    if menu then draw_menu() end
    if not menu then draw_game() end
end

function draw_menu()
    local w1 = GetTextWidth("SNAKE", 40)
    SetColor(255, 255, 255, 255)
    local text = "SNAKE"
    local x = arcade.width/2-w1/2-50
    for i=1,#text do
        arcade.SetColor(255, 255, 255, 255)
        arcade.DrawText(text[i], x+math.sin(5*arcade.GetTime()+i)*5, 100+math.cos(5*arcade.GetTime()+i)*10, 40)
        x = x + arcade.GetTextWidth(tostring(text[i]), 20)+20
    end
    DrawText("V2", arcade.width/2+50, 120, 20)
    
    SetColor(0, 255, 0, 255)
    w1 = GetTextWidth("PRESS Z TO PLAY", 30)
    DrawText("PRESS Z TO PLAY", arcade.width/2-w1/2, 350, 30)
    
    SetColor(0, 255, 255, 255)
    w1 = GetTextWidth("Create by: lumaio", 20)
    DrawText("Create by: lumaio", arcade.width/2-w1/2, 250, 20)
    
end

function draw_game()
    if dead then
        local w1 = GetTextWidth("You have died", 40)
        local w2 = GetTextWidth("Score: " .. score, 40)
        local w3 = GetTextWidth("Press Z to restart", 40)
        
        SetColor(164, 255, 255, 255)
        DrawText("You have died", arcade.width/2-w1/2, 140, 40)
        DrawText("Score " .. score, arcade.width/2-w2/2, 180, 40)
        SetColor(164, 255, 164, 255)
        DrawText("Press Z to restart", arcade.width/2-w3/2, 220, 40)
        return
    end
    
    for i=1,width*height do
        local j = i-1
        local x = math.floor(j/width)
        local y = j%width
        local t = map[i]
        
        if t.type == "none" then
            SetColor(255, 255, 255, 255)
        elseif t.type == "food" then
            SetColor(0, 255, 0, 255)
        end
        for _,v in ipairs(snake) do
            if v.x == x and v.y == y then
                SetColor(0, 0, 255, 255)
                break
            end
        end
        if x == snake[1].x and y == snake[1].y then
            SetColor(255, 0, 0, 255)
        end
        
        DrawRectangle("fill", 512/2-center+x*20, 75+y*20, 15, 15)
    end
    local w2 = GetTextWidth("Score: " .. score, 40)
    SetColor(164, 255, 164, 255)
    DrawText("Score " .. score, arcade.width/2-w2/2, 450, 40)
    
    if not arcade.interacting then
        local w1 = GetTextWidth("PAUSED", 40)
        SetColor(0, 0, 0, 255)
        DrawRectangle("fill", arcade.width/2-w1/2-10, arcade.height/2-50, w1+20, 60)
        SetColor(255, 255, 255, 255)
        DrawText("PAUSED", arcade.width/2-w1/2, arcade.height/2-40, 40)
    end
end
