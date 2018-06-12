package com.antell.cloudhands.api.packet.udp.dns;

/**
 * CNAME Record  - maps an alias to its real name
 */

public class CNAMERecord extends SingleCompressedNameBase {

    public CNAMERecord() {
    }

    @Override
    public Record getObject() {
        return new CNAMERecord();
    }



    /**
     * Creates a new CNAMERecord with the given data
     *
     * @param alias The name to which the CNAME alias points
     */
    public CNAMERecord(Name name, int dclass, long ttl, Name alias) {
        super(name, Type.CNAME, dclass, ttl, alias, "alias");
    }

    /**
     * Gets the target of the CNAME Record
     */
    public Name getTarget() {
        return getSingleName();
    }

    /**
     * Gets the alias specified by the CNAME Record
     */
    public Name getAlias() {
        return getSingleName();
    }

}
