
conout("{555555ff}console initialization stage")
local i = 0

local commands = {
    "console_check(t) : {aaaaaaff}test output with value 't'",
    "help() : {aaaaaaff}output this help text",
    "conout(t) : {aaaaaaff}output to console with string 't'",
    "clear() : {aaaaaaff}clear console",
    "arcade_reload(t) : {aaaaaaff}reload arcade entity with id 't'"
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
        conout("{ff5555ff}"..err)
    else
        conout("{55ff55ff}reloaded arcade")
    end
end

