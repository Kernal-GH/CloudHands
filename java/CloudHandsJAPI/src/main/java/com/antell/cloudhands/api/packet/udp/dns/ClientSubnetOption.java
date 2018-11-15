
package com.antell.cloudhands.api.packet.udp.dns;

import java.net.InetAddress;
import java.net.UnknownHostException;

/**
 * The Client Subnet EDNS Option, defined in
 * http://tools.ietf.org/html/draft-vandergaast-edns-client-subnet-00
 * ("Client subnet in DNS requests").
 *
 * The option is used to convey information about the IP address of the
 * originating client, so that an authoritative server can make decisions
 * based on this address, rather than the address of the intermediate
 * caching name server.
 *
 * The option is transmitted as part of an OPTRecord in the additional section
 * of a DNS message, as defined by RFC 2671 (EDNS0).
 * 
 * An option code has not been assigned by IANA; the value 20730 (used here) is
 * also used by several other implementations.
 *
 * The wire format of the option contains a 2-byte length field (1 for IPv4, 2
 * for IPv6), a 1-byte source netmask, a 1-byte scope netmask, and an address
 * truncated to the source netmask length (where the final octet is padded with
 * bits set to 0)
 * 
 *
 * @see OPTRecord
 * 
 * @author Brian Wellington
 * @author Ming Zhou &lt;mizhou@bnivideo.com&gt;, Beaumaris Networks
 */
public class ClientSubnetOption extends EDNSOption {


    private int family;
    private int sourceNetmask;
    private int scopeNetmask;
    private InetAddress address;


    public ClientSubnetOption() {
	super(EDNSOption.Code.CLIENT_SUBNET);
}

	private static int checkMaskLength(String field, int family, int val) {

    	int max = Address.addressLength(family) * 8;

    	if (val < 0 || val > max)
			throw new IllegalArgumentException("\"" + field + "\" " + val +
						   " must be in the range " +
						   "[0.." + max + "]");

    	return val;

    }


    /**
     * Returns the family of the network address.  This will be either IPv4 (1)
     * or IPv6 (2).
     */
    public int
    getFamily() {
                      return family;
                                    }

    /** Returns the source netmask. */
    public int
    getSourceNetmask() {
                             return sourceNetmask;
                                                  }

    /** Returns the scope netmask. */
    public int
    getScopeNetmask() {
                            return scopeNetmask;
                                                }

    /** Returns the IP address of the client. */
    public InetAddress
    getAddress() {
                       return address;
                                      }


	void optionRead(DNSDataInput in) throws ParseException {

    	family = in.readU16();

    	if (family != Address.IPv4 && family != Address.IPv6)
			throw new ParseException("unknown address family");

    	sourceNetmask = in.readU8();

    	if (sourceNetmask > Address.addressLength(family) * 8)
			throw new ParseException("invalid source netmask");

    	scopeNetmask = in.readU8();

    	if (scopeNetmask > Address.addressLength(family) * 8)
    		throw new ParseException("invalid scope netmask");

    	// Read the truncated address
		byte [] addr = in.readByteArray();
		if (addr.length != (sourceNetmask + 7) / 8)
			throw new ParseException("invalid address");
		// Convert it to a full length address.
		byte [] fulladdr = new byte[Address.addressLength(family)];
		System.arraycopy(addr, 0, fulladdr, 0, addr.length);

        try {
            address = InetAddress.getByAddress(fulladdr);
        } catch (UnknownHostException e) {
            throw new ParseException("invalid address", e);
        }

        InetAddress tmp = Address.truncate(address, sourceNetmask);
        if (!tmp.equals(address))
            throw new ParseException("invalid padding");
    }

    public String optionToString() {
        StringBuffer sb = new StringBuffer();
        sb.append(address.getHostAddress());
        sb.append("/");
        sb.append(sourceNetmask);
        sb.append(", scope netmask ");
        sb.append(scopeNetmask);
        return sb.toString();
    }

}
