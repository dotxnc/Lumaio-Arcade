
local ClearColor = arcade.ClearColor
local SetColor = arcade.SetColor
local DrawRectangle = arcade.DrawRectangle
local IsKeyDown = arcade.IsKeyDown
local IsKeyPressed = arcade.IsKeyPressed

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

local lazers = {}
local list = {}
local ticker = 0
local last = "right"
local dir = "right"
local death = false
local dead = false

function init()
    conout("{00ffffff}[INVADERS] {ffffffff}initializing...")
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
end

function update(dt)
    ticker = ticker + dt
    if ticker > 0.2 then
        move()
        ticker = 0
    end
    
    if IsKeyDown(arcade.KEY_LEFT) then
        player.x = player.x - 200 * dt
    end
    if IsKeyDown(arcade.KEY_RIGHT) then
        player.x = player.x + 200 * dt
    end
    
    if IsKeyPressed(arcade.KEY_X) then
        table.insert(lazers, {
            x = player.x+7*spritesize,
            y = arcade.height-50
            })
    end
    
    for i,v in ipairs(lazers) do
        v.y = v.y - 250 * dt
        for k,z in ipairs(list) do
            if z.y == nil then conout("z.y nil") end
            if v.x > z.x and v.x < z.x + 22 and v.y > z.y and v.y < z.y + 16 then
                table.remove(list, k)
                table.remove(lazers, i)
                break
            end
        end
    end
    
end

function draw()
    SetColor(25, 25, 25, 255)
    DrawRectangle("fill", 0, 0, arcade.width, arcade.height)
    
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
end
