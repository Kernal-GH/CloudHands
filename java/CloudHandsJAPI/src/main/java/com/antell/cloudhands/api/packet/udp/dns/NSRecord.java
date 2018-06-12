
package com.antell.cloudhands.api.packet.udp.dns;

/**
 * Name Server Record  - contains the name server serving the named zone
 */

public class NSRecord extends SingleCompressedNameBase {

    private static final long serialVersionUID = 487170758138268838L;

   public NSRecord() {
    }

    @Override
    public Record getObject() {
        return new NSRecord();
    }

    /**
     * Creates a new NS Record with the given data
     *
     * @param target The name server for the given domain
     */
    public NSRecord(Name name, int dclass, long ttl, Name target) {
        super(name, Type.NS, dclass, ttl, target, "target");
    }

    /**
     * Gets the target of the NS Record
     */
    public Name getTarget() {
        return getSingleName();
    }

    @Override
    public Name getAdditionalName() {
        return getSingleName();
    }

}
