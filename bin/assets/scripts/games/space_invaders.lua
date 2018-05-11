
-- [[ TODO: alien lazers ]]
-- [[ TODO: barriers ]]

-- [[ DONE: end scren ]]

local ClearColor = arcade.ClearColor
local SetColor = arcade.SetColor
local DrawRectangle = arcade.DrawRectangle
local IsKeyDown = arcade.IsKeyDown
local IsKeyPressed = arcade.IsKeyPressed
local DrawText = arcade.DrawText

local spritesize = 2

local aliens = {
    {
        color = {255,255,255,255},
        segments = {
            { 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0 },
            { 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1 },
            { 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1 },
            { 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1 },
            { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
            { 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0 },
            { 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0 },
            { 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0 },
        }
    }
}

local player = {
    x = 100,
    y = arcade.height-50,
    ticker = 0,
    segments = {
        { 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0 },
        { 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0 },
        { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
        { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
        { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
    }
}

local zappers = {}
local lazers = {}
local list = {}
local ticker = 0
local last = "right"
local dir = "right"
local death = false
local dead = false
local speed = 1
local score = 0
local wave = 1
local lives = 3

function init()
    conout("{00ffffff}[INVADERS] {ffffffff}initializing...")
    
    math.randomseed(arcade.GetTime())
    
    zappers = {}
    lazers = {}
    list = {}
    ticker = 0
    player.ticker = 0
    last = "right"
    dir = "right"
    dead = false
    death = false
    speed = 1-((wave-1)*100)/100
    
    for i=1,4 do
        for j=1,10 do
            table.insert(list, {id=1, x=j*30, y=i*30})
        end
    end
end

local function move()
    local left = list[1]
    local right = list[1]
    local bottom = list[1]
    
    for i,v in ipairs(list) do
        if v.x < left.x then left = v end
        if v.x > right.x then right = v end
        if v.y > bottom.y then bottom = v end
    end
    
    if left == nil or right == nil or bottom == nil then return end
    if dir == "right" then
        if right.x + 22 > arcade.width-50 then
            dir = "down"
            last = "right"
            move()
        else
            for i,v in ipairs(list) do
                v.x = v.x + 22
            end
        end
    elseif dir == "down" then
        if bottom.y + 16 > arcade.height-150 then
            if death then dead = true else death = true end
        else
            for i,v in ipairs(list) do
                v.y = v.y + 16
            end
        end
        
        if last == "right" then
            dir = "left"
        else
            dir = "right"
        end
    elseif dir == "left" then
        if left.x + 22 < 60 then
            dir = "down"
            last = "left"
            move()
        else
            for i,v in ipairs(list) do
                v.x = v.x - 22
            end
        end
    end
    
    
    for i,v in ipairs(list) do
        if #zappers < 4+wave*2 then
            if i % math.random(#list) == 0 then
                local r = math.random(100)
                if r < wave*2 then
                    table.insert(zappers, { x = v.x+7*spritesize, y = v.y+16 })
                end
            end
        end
    end
    
end

function update(dt)
    if not arcade.interacting or dead then return end
    
    player.ticker = player.ticker + dt
    
    if IsKeyDown(arcade.KEY_LEFT) then
        player.x = player.x - 200 * dt
    end
    if IsKeyDown(arcade.KEY_RIGHT) then
        player.x = player.x + 200 * dt
    end
    
    if IsKeyPressed(arcade.KEY_X) and player.ticker > 0.2 then
        table.insert(lazers, { x = player.x+7*spritesize, y = arcade.height-50 })
        player.ticker = 0
    end
    
    for i,v in ipairs(lazers) do
        v.y = v.y - 400 * dt
        for k,z in ipairs(list) do
            if v.x > z.x and v.x < z.x + 22 and v.y > z.y and v.y < z.y + 16 then
                table.remove(list, k)
                table.remove(lazers, i)
                speed = speed * 0.95
                score = score + 1 * wave
                break
            end
        end
    end
    
    for i,v in ipairs(zappers) do
        v.y = v.y + 300 * dt
        if v.x > player.x and v.x < player.x + 22 and v.y > player.y and v.y < player.y + 16 then
            table.remove(zappers, i)
            lives = lives - 1
        end
        if v.y > arcade.height then
            table.remove(zappers, i)
        end
    end
    
    ticker = ticker + dt
    if ticker > speed then
        move()
        ticker = 0
    end
    
    if lives <= 0 then
        dead = true
    end
    
    if #list == 0 and #zappers == 0 then
        wave = wave + 1
        init()
    end
end

function draw()
    SetColor(25, 25, 25, 255)
    DrawRectangle("fill", 0, 0, arcade.width, arcade.height)
    
    if dead then
        local w1 = arcade.GetTextWidth("You have died", 40)
        local w2 = arcade.GetTextWidth("Score: " .. score, 40)
        local w3 = arcade.GetTextWidth("Press Z to restart", 40)
        
        SetColor(164, 255, 255, 255)
        DrawText("You have died", arcade.width/2-w1/2, 140, 40)
        DrawText("Score " .. score, arcade.width/2-w2/2, 180, 40)
        SetColor(164, 255, 164, 255)
        DrawText("Press Z to restart", arcade.width/2-w3/2, 220, 40)
        
        if IsKeyPressed(arcade.KEY_Z) then
            init()
            score = 0
            wave = 1
            lives = 3
            player.x = arcade.width/2-11
            player.y = arcade.height-50
        end
        
        goto int
    end
    
    SetColor(0, 255, 0, 255)
    for z=1,lives do
        for k,i in ipairs(player.segments) do
            for h,j in ipairs(i) do
                if i[h] == 1 then
                    DrawRectangle("fill", arcade.width-150+h+z*20, 25+k, 1, 1)
                end
            end
        end
    end
    
    for z,v in ipairs(list) do
        local color = aliens[v.id].color
        SetColor(color[1], color[2], color[3], color[4]);
        for k,i in ipairs(aliens[v.id].segments) do
            for h,j in ipairs(i) do
                if i[h] == 1 then
                    DrawRectangle("fill", v.x+h*spritesize, v.y+k*spritesize, spritesize, spritesize)
                end
            end
        end
    end
    
    SetColor(0, 255, 0, 255)
    for k,i in ipairs(player.segments) do
        for h,j in ipairs(i) do
            if i[h] == 1 then
                DrawRectangle("fill", player.x+h*spritesize, arcade.height-50+k*spritesize, spritesize, spritesize)
            end
        end
    end
    
    for i,v in ipairs(lazers) do
        DrawRectangle("fill", v.x-1, v.y-2.5, 2, 5)
    end
    
    SetColor(255, 255, 255, 255)
    for i,v in ipairs(zappers) do
        DrawRectangle("fill", v.x-1, v.y-2.5, 2, 5)
    end
    
    ::int::
    if not arcade.interacting then
        local w1 = arcade.GetTextWidth("PAUSED", 40)
        SetColor(0, 0, 0, 255)
        DrawRectangle("fill", arcade.width/2-w1/2-10, arcade.height/2-50, w1+20, 60)
        SetColor(255, 255, 255, 255)
        DrawText("PAUSED", arcade.width/2-w1/2, arcade.height/2-40, 40)
    end
end
