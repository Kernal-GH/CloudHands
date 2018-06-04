-- by shajf

local ffi = require 'ffi'
local base = require 'base'
local new_tab = base.new_tab

local _M = new_tab(01)

_M.FFI_ETH_P_LOOP      = 0x0060          -- Ethernet Loopback packet     
_M.FFI_ETH_P_PUP       = 0x0200          -- Xerox PUP packet             
_M.FFI_ETH_P_PUPAT     = 0x0201          -- Xerox PUP Addr Trans packet  
_M.FFI_ETH_P_IP        = 0x0800          -- Internet Protocol packet     
_M.FFI_ETH_P_X25       = 0x0805          -- CCITT X.25                   
_M.FFI_ETH_P_ARP       = 0x0806          -- Address Resolution packet    
_M.FFI_ETH_P_BPQ       = 0x08FF          -- G8BPQ AX.25 Ethernet Packet  [ NOT AN OFFICIALLY REGISTERED ID ] 
_M.FFI_ETH_P_IEEEPUP   = 0x0a00          -- Xerox IEEE802.3 PUP packet 
_M.FFI_ETH_P_IEEEPUPAT = 0x0a01          -- Xerox IEEE802.3 PUP Addr Trans packet 
_M.FFI_ETH_P_BATMAN    = 0x4305          -- B.A.T.M.A.N.-Advanced packet [ NOT AN OFFICIALLY REGISTERED ID ] 
_M.FFI_ETH_P_DEC       = 0x6000          -- DEC Assigned proto           
_M.FFI_ETH_P_DNA_DL    = 0x6001          -- DEC DNA Dump/Load            
_M.FFI_ETH_P_DNA_RC    = 0x6002          -- DEC DNA Remote Console       
_M.FFI_ETH_P_DNA_RT    = 0x6003          -- DEC DNA Routing              
_M.FFI_ETH_P_LAT       = 0x6004          -- DEC LAT                      
_M.FFI_ETH_P_DIAG      = 0x6005          -- DEC Diagnostics              
_M.FFI_ETH_P_CUST      = 0x6006          -- DEC Customer use             
_M.FFI_ETH_P_SCA       = 0x6007          -- DEC Systems Comms Arch       
_M.FFI_ETH_P_TEB       = 0x6558          -- Trans Ether Bridging         
_M.FFI_ETH_P_RARP      = 0x8035          -- Reverse Addr Res packet      
_M.FFI_ETH_P_ATALK     = 0x809B          -- Appletalk DDP                
_M.FFI_ETH_P_AARP      = 0x80F3          -- Appletalk AARP               
_M.FFI_ETH_P_8021Q     = 0x8100          -- 802.1Q VLAN Extended Header  
_M.FFI_ETH_P_IPX       = 0x8137          -- IPX over DIX                 
_M.FFI_ETH_P_IPV6      = 0x86DD          -- IPv6 over bluebook           
_M.FFI_ETH_P_PAUSE     = 0x8808          -- IEEE Pause frames. See 802.3 31B 
_M.FFI_ETH_P_SLOW      = 0x8809          -- Slow Protocol. See 802.3ad 43B 
_M.FFI_ETH_P_WCCP      = 0x883E          -- Web-cache coordination protocol
                                         -- defined in draft-wilson-wrec-wccp-v2-00.txt 
_M.FFI_ETH_P_MPLS_UC   = 0x8847          -- MPLS Unicast traffic         
_M.FFI_ETH_P_MPLS_MC   = 0x8848          -- MPLS Multicast traffic       
_M.FFI_ETH_P_ATMMPOA   = 0x884c          -- MultiProtocol Over ATM       
_M.FFI_ETH_P_PPP_DISC  = 0x8863          -- PPPoE discovery messages     
_M.FFI_ETH_P_PPP_SES   = 0x8864          -- PPPoE session messages       
_M.FFI_ETH_P_LINK_CTL  = 0x886c          -- HPNA wlan link local tunnel 
_M.FFI_ETH_P_ATMFATE   = 0x8884          -- Frame-based ATM Transport
                                         -- over Ethernet

_M.FFI_ETH_P_PAE       = 0x888E          -- Port Access Entity (IEEE 802.1X) 
_M.FFI_ETH_P_AOE       = 0x88A2          -- ATA over Ethernet            
_M.FFI_ETH_P_8021AD    = 0x88A8          -- 802.1ad Service VLAN         
_M.FFI_ETH_P_802_EX1   = 0x88B5          -- 802.1 Local Experimental 1.  
_M.FFI_ETH_P_TIPC      = 0x88CA          -- TIPC                         
_M.FFI_ETH_P_8021AH    = 0x88E7          -- 802.1ah Backbone Service Tag 
_M.FFI_ETH_P_MVRP      = 0x88F5          -- 802.1Q MVRP                  
_M.FFI_ETH_P_1588      = 0x88F7          -- IEEE 1588 Timesync 
_M.FFI_ETH_P_PRP       = 0x88FB          -- IEC 62439-3 PRP/HSRv0        
_M.FFI_ETH_P_FCOE      = 0x8906          -- Fibre Channel over Ethernet  
_M.FFI_ETH_P_TDLS      = 0x890D          -- TDLS 
_M.FFI_ETH_P_FIP       = 0x8914          -- FCoE Initialization Protocol 
_M.FFI_ETH_P_80221     = 0x8917          -- IEEE 802.21 Media Independent Handover Protocol 
_M.FFI_ETH_P_LOOPBACK  = 0x9000          -- Ethernet loopback packet per IEEE 802.3 
_M.FFI_ETH_P_QINQ1     = 0x9100          -- deprecated QinQ VLAN [ NOT AN OFFICIALLY REGISTERED ID ] 
_M.FFI_ETH_P_QINQ2     = 0x9200          -- deprecated QinQ VLAN [ NOT AN OFFICIALLY REGISTERED ID ] 
_M.FFI_ETH_P_QINQ3     = 0x9300          -- deprecated QinQ VLAN [ NOT AN OFFICIALLY REGISTERED ID ] 
_M.FFI_ETH_P_EDSA      = 0xDADA          -- Ethertype DSA [ NOT AN OFFICIALLY REGISTERED ID ] 
_M.FFI_ETH_P_AF_IUCV   = 0xFBFB          -- IBM af_iucv [ NOT AN OFFICIALLY REGISTERED ID ] 

_M.FFI_ETH_P_802_3_MIN = 0x0600          -- If the value in the ethernet type is less than this value
                                         -- then the frame is Ethernet II. Else it is 802.3 

_M.FFI_ETH_LENGTH_MAX = 0x05DC

-- Standard well-defined IP protocols.  
_M.FFI_IPPROTO_IP = 0               -- Dummy protocol for TCP               
_M.FFI_IPPROTO_ICMP = 1             -- Internet Control Message Protocol    
_M.FFI_IPPROTO_IGMP = 2             -- Internet Group Management Protocol   
_M.FFI_IPPROTO_IPIP = 4             -- IPIP tunnels (older KA9Q tunnels use 94) 
_M.FFI_IPPROTO_TCP = 6              -- Transmission Control Protocol        
_M.FFI_IPPROTO_EGP = 8              -- Exterior Gateway Protocol            
_M.FFI_IPPROTO_PUP = 12             -- PUP protocol                         
_M.FFI_IPPROTO_UDP = 17             -- User Datagram Protocol               
_M.FFI_IPPROTO_IDP = 22             -- XNS IDP protocol                     
_M.FFI_IPPROTO_TP = 29              -- SO Transport Protocol Class 4        
_M.FFI_IPPROTO_DCCP = 33            -- Datagram Congestion Control Protocol 
_M.FFI_IPPROTO_IPV6 = 41            -- IPv6-in-IPv4 tunnelling              
_M.FFI_IPPROTO_RSVP = 46            -- RSVP Protocol                        
_M.FFI_IPPROTO_GRE = 47             -- Cisco GRE tunnels (rfc 17011702)    
_M.FFI_IPPROTO_ESP = 50             -- Encapsulation Security Payload protocol 
_M.FFI_IPPROTO_AH = 51              -- Authentication Header protocol       
_M.FFI_IPPROTO_MTP = 92             -- Multicast Transport Protocol         
_M.FFI_IPPROTO_BEETPH = 94          -- IP option pseudo header for BEET     
_M.FFI_IPPROTO_ENCAP = 98           -- Encapsulation Header                 
_M.FFI_IPPROTO_PIM = 103            -- Protocol Independent Multicast       
_M.FFI_IPPROTO_COMP = 108           -- Compression Header Protocol          
_M.FFI_IPPROTO_SCTP = 132           -- Stream Control Transport Protocol    
_M.FFI_IPPROTO_UDPLITE = 136        -- UDP-Lite (RFC 3828)                  
_M.FFI_IPPROTO_MPLS = 137           -- MPLS in IP (RFC 4023)                
_M.FFI_IPPROTO_RAW = 255            -- Raw IP packets                       

return _M

