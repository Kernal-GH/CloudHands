local ffi = require "ffi"
local List = require "pl.List"
local class = require "pl.class"

-- struct
ffi.cdef[[
    typedef struct ch_stat_value_t ch_stat_value_t;

    struct ch_stat_value_t {
        
        uint64_t req_packets;
        uint64_t req_bytes;
        uint64_t req_start_time;
        uint64_t req_last_time;

        uint64_t res_packets;
        uint64_t res_bytes;
        uint64_t res_start_time;
        uint64_t res_last_time;

    };
]]

local function stat_value_init(stv) 
    stv.req_packets = 0
    stv.req_bytes = 0
    stv.req_start_time = 0
    stv.req_last_time = 0
    
    stv.res_packets = 0
    stv.res_bytes = 0
    stv.res_start_time = 0
    stv.res_last_time = 0
end

local function to_string(v)

    local s = "{req_packets:" .. tostring(v.req_packets) .."," .. "req_bytes:" .. tostring(v.req_bytes) .. "," .. "req_start_time:" .. tostring(v.req_start_time) .. "," .. "req_last_time:" .. tostring(v.req_last_time) .. "," 
    s = s .. "res_packets:" .. tostring(v.res_packets) .."," .. "res_bytes:" .. tostring(v.res_bytes) .. "," .. "res_start_time:" .. tostring(v.res_start_time) .. "," .. "res_last_time:" .. tostring(v.res_last_time) .. "}"

    return s
end

local StatValuePool = class {

    _init = function(self,pre_alloc_n)

        self.pool = List({})
        for i=1,pre_alloc_n do
            stv = ffi.new("ch_stat_value_t")
            stat_value_init(stv)
            self.pool:append(stv)

        end

    end,

    alloc = function(self)
        local v =  self.pool:pop()
        if v ==nil then
            v = ffi.new("ch_stat_value_t")
            stat_value_init(v)
        end
        return v
    end,

    free = function(self,v)
        stat_value_init(v)
        self.pool:append(v)
    end,

    __tostring = function(self)
        res = "["
        for i,v in pairs(self.pool) do
            res = res .. to_string(v) .. ","
        end
        res = res.."]\n"
        return res
    end,

    vtostring = function(self,v) 
        return to_string(v)
    end
}

return StatValuePool

