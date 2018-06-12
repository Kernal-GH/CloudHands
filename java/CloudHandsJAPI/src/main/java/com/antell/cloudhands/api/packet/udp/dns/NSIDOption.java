package com.antell.cloudhands.api.packet.udp.dns;

/**
 * The Name Server Identifier Option, define in RFC 5001.
 *
 * @see OPTRecord
 */
public class NSIDOption extends GenericEDNSOption {

    private static final long serialVersionUID = 74739759292589056L;

    NSIDOption() {
        super(EDNSOption.Code.NSID);
    }

    /**
     * Construct an NSID option.
     *
     * @param data The contents of the option.
     */
    public NSIDOption(byte[] data) {
        super(EDNSOption.Code.NSID, data);
    }

}
