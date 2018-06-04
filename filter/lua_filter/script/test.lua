-- by shajf
local ffi = require 'ffi'
local C = ffi.C
local ipv4 = require 'packet_ipv4'
local getfenv = getfenv


function chlua_check_init()


end


function chlua_check()

	local iph = ipv4.get_ipv4_hdr()

	print(iph.srcIP)
	print(iph.dstIP)

	return 0

end
