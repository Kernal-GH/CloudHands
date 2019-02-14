local _M = {}

local ffi = require "ffi"

ffi.cdef[[

    struct timeval {
        long int tv_sec;
        long int tv_usec;
    };
    int gettimeofday(struct timeval *tv, void *tz);
]]

local tm = ffi.new("struct timeval");

-- 返回毫秒级时间戳
function _M.current_time_millis()   
    ffi.C.gettimeofday(tm,nil);
    local sec =  tonumber(tm.tv_sec*1000);
    local usec =  tonumber(tm.tv_usec/1000);
    return sec + usec;
end

return _M
