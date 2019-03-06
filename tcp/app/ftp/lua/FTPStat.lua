
local ffi = require "ffi"
local cast = ffi.cast
local ctostring = ffi.string

local StatCount =  require "StatCount"
local getfenv = getfenv

ffi.cdef[[

    typedef struct ch_ftp_stat_data_t ch_ftp_stat_data_t;

    struct ch_ftp_stat_data_t {

        const char *key;
        uint64_t count;
    };
]]

local ftp_stat_env = {}

function ftp_stat_init()

    ftp_stat_env.shm_fmt = getfenv(0).__ftp_stat_shm_fmt

    if not ftp_stat_env.shm_fmt then
        return error("Must specify the ftp stat shm fmt!")
    end

    ftp_stat_env.stat = StatCount(60,100)

end

function ftp_stat_run()

    local stat_data = cast("ch_ftp_stat_data_t*",getfenv(0).__ftp_stat_data)

    if not stat_data then

        ftp_stat_env.stat:output()

    else
        local key = ctostring(stat_data.key)

        ftp_stat_env.stat:stat(key,stat_data.count)

    end


end


function ftp_stat_fin()

    -- do nothing

end

