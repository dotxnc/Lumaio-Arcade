local menu = {}
menu.__index = menu
menu = setmetatable(menu, {
    __call = function(cls, ...)
        return cls.new(...)
    end
})

function menu.new(title)
    local self = setmetatable({}, menu)
    self.title = title
    return self
end

function menu:update(dt)

end    

function menu:draw()
    
end
