
getmetatable('').__index = function(str,i) return string.sub(str,i,i) end

local text = "Hello from lua!"

local colors = {
    {255, 0,   0,   255}, -- red
    {255, 127, 0,   255}, -- orange
    {255, 255, 0,   255}, -- yellow
    {0,   255, 0,   255}, -- green
    {0,   0,   255, 255}, -- blue
    {75,  0,   130, 255}, -- indigo
    {148, 0,   211, 255}, -- violet
}

function init()
    conout("{00ffffff}[TEST] {ffffffff}test script init")
    arcade.LoadTexture("assets/textures/Arcade1_texture.png", "Arcade1_texture")
end

function update(dt)
    
end

function draw()
    arcade.SetColor(255, 255, 255, 255)
    arcade.DrawText("Interacting: " .. tostring(arcade.interacting), 10, 10, 20)
    arcade.DrawText("KEY_X      : " .. tostring(arcade.IsKeyDown(arcade.KEY_X)), 10, 30, 20)
    
    arcade.SetColor(255, 255, 0, 255)
    local x = 20
    for i=1,#text do
        local c = colors[i%#colors+1]
        arcade.SetColor(c[1], c[2], c[3], c[4])
        arcade.DrawText(text[i], x+math.sin(5*arcade.GetTime()+i)*5, 100+math.cos(5*arcade.GetTime()+i)*10, 40)
        x = x + arcade.GetTextWidth(tostring(text[i]), 20)+20
    end
    
    arcade.SetColor(255 ,255, 255, 255)
    arcade.DrawTexture("Arcade1_texture", arcade.width/2+math.cos(arcade.GetTime())*arcade.width/2, 0, 0.1)
end
