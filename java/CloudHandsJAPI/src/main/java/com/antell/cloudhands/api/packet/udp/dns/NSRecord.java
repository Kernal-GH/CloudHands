
package com.antell.cloudhands.api.packet.udp.dns;

/**
 * Name Server Record  - contains the name server serving the named zone
 */

public class NSRecord extends SingleCompressedNameBase {


   public NSRecord() {
    }

    @Override
    public Record getObject() {
        return new NSRecord();
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
