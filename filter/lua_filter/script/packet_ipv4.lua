-- by shajf

local ffi = require 'ffi'
local base = require 'base'
local new_tab = base.new_tab
local C = ffi.C
local ffi_cast = ffi.cast
local ffi_str = ffi.string
local get_string_buf = base.get_string_buf
local get_size_ptr = base.get_size_ptr
local getfenv = getfenv
local type = type
local error = error



local _M = new_tab(0,1)

ffi.cdef[[
	typedef struct {
		
		uint8_t  vihl;	
		uint8_t  tos;	
		uint16_t tlen;		
		uint16_t pid;		
		uint16_t fragOff;
		uint8_t  ttl;
		uint8_t  protoID;
		uint16_t checksum;
		uint32_t srcIP;
		uint32_t dstIP;

	} ch_packet_ipv4_t;

	int ch_packet_ipv4_hdr_get(ch_packet_t *pkt,ch_packet_ipv4_t *ipv4);
]]

local ipv4_type = ffi.typeof("ch_packet_ipv4_t*")
local ipv4_size = ffi.sizeof("ch_packet_ipv4_t")

function _M.get_ipv4_hdr()

    local packet = getfenv(0).__ch_packet

    if not packet then
        return error("no packet found")
    end
    
	local ipv4_buf = get_string_buf(ipv4_size)
    local ipv4_ptr = ffi_cast(ipv4_type,ipv4_buf)

    local rc = C.ch_packet_ipv4_hdr_get(packet,ipv4_ptr)

    if rc == FFI_OK then
	
		return ipv4_ptr
	end

	return {}

end

return _M

