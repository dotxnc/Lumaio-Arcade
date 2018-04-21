
local ClearColor = arcade.ClearColor
local SetColor = arcade.SetColor
local DrawRectangle = arcade.DrawRectangle

local spritesize = 2

local aliens = {
    {
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

local list = {}

function init()
    conout("{00ffffff}[INVADERS] {ffffffff}initializing...")
    for i=1,10 do
        table.insert(list, {id=1, x=i*30, y=50})
    end
end

function update(dt)
    
end

function draw()
    --ClearColor(25, 25, 25, 255)
    SetColor(25, 25, 25, 255)
    DrawRectangle("fill", 0, 0, arcade.width, arcade.height)
    
    for z,v in ipairs(list) do
        for k,i in ipairs(aliens[v.id].segments) do
            for h,j in ipairs(i) do
                if i[h] == 1 then
                    SetColor(255, 255, 255, 255)
                    DrawRectangle("fill", v.x+h*spritesize, v.y+k*spritesize, spritesize, spritesize)
                end
            end
        end
    end
end
