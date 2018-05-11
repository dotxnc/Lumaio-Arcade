
conout("{0x555555}console initialization stage")
local i = 0

local commands = {
    "console_check(t) : {0xaaaaaa}test output with value 't'",
    "help() : {0xaaaaaa}output this help text",
    "conout(t) : {0xaaaaaa}output to console with string 't'",
    "clear() : {0xaaaaaa}clear console",
    "arcade_reload(t) : {0xaaaaaa}reload arcade entity with id 't'"
}

function string.starts(String,Start)
    return string.sub(String,1,string.len(Start))==Start
 end

function console_check(t)
    conout("checking... " .. i .. " : " .. t)
    i=i+1
end

function help()
    for i,v in ipairs(commands) do
        conout(v)
    end
end

function _autocomplete(command)
    local complete = {}
    for key,value in pairs(_G) do
        if (string.starts(key, command)) then
            table.insert(complete, key)
        end
    end
    _complete(complete)
end

function arcade_reload(name)
    local err = _reload(name)
    if err then
        conout("{0xff5555}"..err)
    else
        conout("{0x55ff55}reloaded arcade")
    end
end

