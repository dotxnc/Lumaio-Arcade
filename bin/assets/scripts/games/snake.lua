local map = {}


local F = 1
local N = 2
local E = 3
local S = 4
local W = 5

local head = {x=10,y=8}
local tail = {x=8,y=8}
local elapsed = 0
local direction = E
local score = 0
local grow = 0
local dead = false
local need_update = false

local blue = {0, 0, 255, 255}
local white = {255, 255, 255, 255}
local red = {255, 0, 0, 255}
local green = {0, 255, 0, 255}

function initialize_snake()
    print("initializing snake...")
    
    dead = false
    elapsed = 0
    grow = 0
    direction = E
    need_update = false
    score = 0
    
    head = {x=10,y=8}
    tail = {x=8,y=8}
    
    map = {}
    for i = 1, 16 do
        map[i] = {}
        for j = 1, 16 do
            map[i][j] = 0
        end
    end
    
    for i=tail.x,head.x do
        map[i][tail.y] = E
    end

    local x = math.random(1, 16)
    local y = math.random(1, 16)
    while map[x][y] > 0 do
        x = math.random(1, 16)
        y = math.random(1, 16)
    end
    map[x][y] = F
end

function update(dt)
    if not arcade.interacting then return end
    
    elapsed = elapsed + dt
    
    if not dead then
        if (arcade.IsKeyPressed(arcade.KEY_UP) and direction ~= S and direction ~= N) then
            elapsed = 0
            map[head.x][head.y] = N
            head.y = head.y - 1
            direction = N
            need_update = true
        end
        if (arcade.IsKeyPressed(arcade.KEY_DOWN) and direction ~= S and direction ~= N) then
            elapsed = 0
            map[head.x][head.y] = S
            head.y = head.y + 1
            direction = S
            need_update = true
        end
        if (arcade.IsKeyPressed(arcade.KEY_LEFT) and direction ~= E and direction ~= W) then
            elapsed = 0
            map[head.x][head.y] = W
            head.x = head.x - 1
            direction = W
            need_update = true
        end
        if (arcade.IsKeyPressed(arcade.KEY_RIGHT) and direction ~= E and direction ~= W) then
            elapsed = 0
            map[head.x][head.y] = E
            head.x = head.x + 1
            direction = E
            need_update = true
        end
        if (head.x<1 or head.x>16 or head.y<1 or head.y>16 or (need_update and map[head.x][head.y] > 1)) then
            dead = true
            -- TODO: highscore
        end
    else
        if (arcade.IsKeyPressed(arcade.KEY_Z)) then
            initialize_snake()
        end
    end
    
    if elapsed > 0.1 then
        elapsed = 0
        if direction == N then
            if head.y-1<1 then
                dead = true
                return
            end
            map[head.x][head.y] = N
            head.y = head.y - 1
            direction = N
            if map[head.x][head.y] > 1 then
                dead = true
                return
            end
        elseif direction == S then
            if head.y+1>16 then
                dead = true
                return
            end
            map[head.x][head.y] = S
            head.y = head.y + 1
            direction = S
            if map[head.x][head.y] > 1 then
                dead = true
                return
            end
        elseif direction == E then
            if head.x+1>16 then
                dead = true
                return
            end
            map[head.x][head.y] = E
            head.x = head.x + 1
            direction = E
            if map[head.x][head.y] > 1 then
                dead = true
                return
            end
        elseif direction == W then
            if head.x-1<1 then
                dead = true
                return
            end
            map[head.x][head.y] = W
            head.x = head.x - 1
            direction = W
            if map[head.x][head.y] > 1 then
                dead = true
                return
            end
        end
        need_update = true
    end
    
    if need_update and not dead then
        if map[head.x][head.y] == F then
            grow = 1
            score = score + 1
            local x = math.random(1, 16)
            local y = math.random(1, 16)
            while map[x][y] > 0 do
                x = math.random(1, 16)
                y = math.random(1, 16)
            end
            map[x][y] = F
        end
        map[head.x][head.y] = 6
        if grow > 0 then
            grow = grow - 1
        else
            local a = tail.x
            local b = tail.y
            local t = map[a][b]
            if t == N then
                tail.y = tail.y - 1
            elseif t == S then
                tail.y = tail.y + 1
            elseif t == E then
                tail.x = tail.x + 1
            elseif t == W then
                tail.x = tail.x - 1
            end
            map[a][b] = 0
        end
        need_update = false
    end
    
end

function draw()
    arcade.ClearColor(25, 25, 25, 255)
    
    if not dead then
        
        for x=1,16 do
            for y=1,16 do
                local c = blue
                if map[x][y] == 0 then c = white end
                if map[x][y] == F then c = green end
                arcade.SetColor(table.unpack(c))
                arcade.DrawRectangle("fill", 78+x*20, 75+y*20, 15, 15)
            end
        end
        local w2 = arcade.GetTextWidth("Score: " .. score, 40)
        arcade.SetColor(164, 255, 164, 255)
        arcade.DrawText("Score " .. score, arcade.width/2-w2/2, 450, 40)
    else
        local w1 = arcade.GetTextWidth("You have died", 40)
        local w2 = arcade.GetTextWidth("Score: " .. score, 40)
        local w3 = arcade.GetTextWidth("Press Z to restart", 40)
        
        arcade.SetColor(164, 255, 255, 255)
        arcade.DrawText("You have died", arcade.width/2-w1/2, 140, 40)
        arcade.DrawText("Score " .. score, arcade.width/2-w2/2, 180, 40)
        arcade.SetColor(164, 255, 164, 255)
        arcade.DrawText("Press Z to restart", arcade.width/2-w3/2, 220, 40)
    end
    
    if not arcade.interacting then
        local w1 = arcade.GetTextWidth("PAUSED", 40)
        arcade.SetColor(0, 0, 0, 255)
        arcade.DrawRectangle("fill", arcade.width/2-w1/2-10, arcade.height/2-50, w1+20, 60)
        arcade.SetColor(255, 255, 255, 255)
        arcade.DrawText("PAUSED", arcade.width/2-w1/2, arcade.height/2-40, 40)
    end
end

initialize_snake()
