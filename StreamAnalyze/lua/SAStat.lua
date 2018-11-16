local ffi = require "ffi"
local cast = ffi.cast
local ctostring = ffi.string

local Stat =  require "Stat"
local getfenv = getfenv


ffi.cdef[[
    typedef struct ch_sa_stat_data_t ch_sa_stat_data_t;
    struct ch_sa_stat_data_t {

        const char *key;
        int is_req;
        uint64_t pkts;
        uint64_t bytes;
    };
]]

local sa_stat_env = {}

local function sa_stat_data_print(data)
    
    local key = ctostring(data.key)

    s = "{key: " ..key .. ",is_req:".. tostring(data.is_req) .. ",pkts:"..tostring(data.pkts) .. ",bytes:" .. tostring(data.bytes) .."}"
    print(s)

end

function sa_stat_init()

    sa_stat_env.shm_fmt = getfenv(0).__sa_stat_shm_fmt

    if not sa_stat_env.shm_fmt then
        return error("Must specify the sa stat shm fmt!")
    end

    sa_stat_env.stat = Stat(60,100)

end

function sa_stat_run()

    local stat_data = cast("ch_sa_stat_data_t*",getfenv(0).__sa_stat_data)

    if not stat_data then

        sa_stat_env.stat:output()

    else
        local key = ctostring(stat_data.key)

        sa_stat_env.stat:stat(key,stat_data.is_req,stat_data.pkts,stat_data.bytes)

    end


end


function sa_stat_fin()

    -- do nothing

end

