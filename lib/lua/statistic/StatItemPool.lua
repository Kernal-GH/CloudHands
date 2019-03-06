
local ffi = require "ffi"
local List = require "pl.List"
local class = require "pl.class"

-- define stat item c struct

ffi.cdef[[
    
    typedef struct ch_stat_item_count_t ch_stat_item_count_t;

    struct ch_stat_item_count_t {

        uint64_t start_time;
        uint64_t last_time;
        uint64_t count;
    };

]]

local function stat_item_count_init(stv)

    stv.start_time = 0
    stv.last_time = 0
    stv.count = 0
end

local stat_item_init_table = {}
stat_item_init_table["ch_stat_item_count_t"] = stat_item_count_init

local function stat_item_init(struct_name,stv)

    local init_fun = stat_item_init_table[struct_name]
    if init_fun ~= nil then

        init_fun(stv)

    end

end


local StatItemPool = class {

    _init = function(self,struct_name,pre_alloc_n)

        self.stat_struct_name = struct_name
        self.pool = List({})

        for i=1,pre_alloc_n do
            stv = ffi.new(struct_name)
            stat_item_init(stv)
            self.pool:append(stv)

        end

    end,

    alloc = function(self)
        local v =  self.pool:pop()
        if v ==nil then
            v = ffi.new(self.stat_struct_name)
            stat_item_init(v)
        end
        return v
    end,

    free = function(self,v)
        stat_item_init(v)
        self.pool:append(v)
    end

}

return StatValuePool

