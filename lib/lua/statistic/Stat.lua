local class = require "pl.class"
local StatValuePool = require "StatValuePool"
local time = require "time" 

local function update_stat(stat_value,is_req,pkts,bytes) 

    t = time.current_time_millis()
    if is_req == 1 then
        stat_value.req_packets = stat_value.req_packets+pkts
        stat_value.req_bytes = stat_value.req_bytes + bytes;
        if stat_value.req_start_time == 0 then
            stat_value.req_start_time = t
        end
        stat_value.req_last_time = t

    else
        stat_value.res_packets = stat_value.res_packets+pkts
        stat_value.res_bytes = stat_value.res_bytes + bytes;
        if stat_value.res_start_time == 0 then
            stat_value.res_start_time = t
        end
        stat_value.res_last_time = t
    end

end

local function isTimeout(tm,stat_value,tv)

    if tm-stat_value.req_start_time>=tv*1000 then
        return true
    else
        return false
    end
end

local Stat = class {

    _init = function(self,stat_tv,pre_alloc_n)

        self.stat_tv = stat_tv
        self.stv_pool = StatValuePool(pre_alloc_n)
        self.stat_table = {}
    end,

    __tostring = function(self)

        res = "stat_tv:" .. tostring(self.stat_tv) .. "\n"
        res = res .. self.stv_pool:__tostring()
        return res
    end,

    output = function(self)

        local tm = time.current_time_millis()
        local count = 0
        for k,v in pairs(self.stat_table) do
            if isTimeout(tm,v,self.stat_tv) then

                print(self.stv_pool:vtostring(v))
                self.stat_table[k] = nil
                self.stv_pool:free(v)
                count = count+1
            end
        end
        return count
    end,

    stat = function(self,key,is_req,pkts,bytes)

        stat_value = self.stat_table[key]
        if stat_value == nil then
            stat_value = self.stv_pool:alloc()
            if stat_value ~= nil then
                self.stat_table[key] = stat_value
            else
                return nil
            end
        end

        update_stat(stat_value,is_req,pkts,bytes)

        return stat_value

    end

}

return Stat



