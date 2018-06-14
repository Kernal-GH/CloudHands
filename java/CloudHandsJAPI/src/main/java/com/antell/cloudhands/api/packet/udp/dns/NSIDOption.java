
package com.antell.cloudhands.api.packet.udp.dns;

/**
 * The Name Server Identifier Option, define in RFC 5001.
 *
 * @see OPTRecord
 * 
 */
public class NSIDOption extends GenericEDNSOption {


	public NSIDOption() {
	super(EDNSOption.Code.NSID);
}

}
