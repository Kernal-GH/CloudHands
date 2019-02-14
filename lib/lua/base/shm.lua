local ffi = require "ffi"
local _M = {}

ffi.cdef[[

    typedef struct ch_shm_record_t ch_shm_record_t;
    typedef struct ch_packet_record_t ch_packet_record_t;

    struct ch_shm_record_t {

        uint32_t magic;
        uint32_t record_size;
        uint32_t data_offset;
        size_t data_len;
        void *data;

    };

    struct ch_packet_record_t {

        ch_shm_record_t shm_rcd;

        uint8_t type;
        uint16_t meta_data_size;
        
        uint64_t time;

    };
    
    extern int ch_shm_format_put(ch_shm_format_t *fmt,ch_shm_record_t *record);
    
    extern void ch_shm_format_flush(ch_shm_format_t *fmt);
]]

function _M.shm_put(shm_fmt,record)

    C.ch_shm_format_put(shm_fmt,record)

end

function _M.shm_flush(shm_fmt)
    C.ch_shm_format_flush(shm_fmt)
end

-- some constants

_M.PKT_RECORD_TYPE_SESSION_TCP = 1
_M.PKT_RECORD_TYPE_SESSION_UDP = 2
_M.PKT_RECORD_TYPE_ICMP = 3
_M.PKT_RECORD_TYPE_ARP = 4
_M.PKT_RECORD_TYPE_OTHER = 5
_M.PKT_RECORD_TYPE_UDP_DNS = 6
_M.PKT_RECORD_TYPE_UDP_TFTP = 7
_M.PKT_RECORD_TYPE_UDP_SMON = 8
_M.PKT_RECORD_TYPE_TCP_HTTP = 9
_M.PKT_RECORD_TYPE_TCP_MAIL = 10
_M.PKT_RECORD_TYPE_TCP_FTP  = 11
_M.PKT_RECORD_TYPE_TCP_TELNET = 12
_M.PKT_RECORD_TYPE_TCP_SMON = 13
_M.PKT_RECORD_TYPE_MREQ_HTTP = 14
_M.PKT_RECORD_TYPE_MREQ_MAIL = 15
_M.PKT_RECORD_TYPE_TCP_FTP_DATA = 16

return _M
