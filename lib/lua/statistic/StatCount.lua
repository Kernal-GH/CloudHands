local class = require "pl.class"
local StatItemPool = require "StatItemPool"
local time = require "time" 

local function update_stat(stat_value,c) 

    t = time.current_time_millis()
        stat_value.count = stat_value.count+c
        if stat_value.start_time == 0 then
            stat_value.start_time = t
        end

        stat_value.last_time = t
end

local function isTimeout(tm,stat_value,tv)

    if tm-stat_value.start_time>=tv*1000 then
        return true
    else
        return false
    end
end

local StatCount = class {

    _init = function(self,stat_tv,pre_alloc_n)

        self.stat_tv = stat_tv
        self.stv_pool = StatItemPool("ch_stat_item_count_t",pre_alloc_n)
        self.stat_table = {}
    end,

    output = function(self)

        local tm = time.current_time_millis()
        local count = 0
        for k,v in pairs(self.stat_table) do
            if isTimeout(tm,v,self.stat_tv) then

                self.stat_table[k] = nil
                self.stv_pool:free(v)
                count = count+1
            end
        end
        return count
    end,

    stat = function(self,key,count)

        stat_value = self.stat_table[key]
        if stat_value == nil then
            stat_value = self.stv_pool:alloc()
            if stat_value ~= nil then
                self.stat_table[key] = stat_value
            else
                return nil
            end
        end

        update_stat(stat_value,count)

        return stat_value

    end

}

return StatCount



