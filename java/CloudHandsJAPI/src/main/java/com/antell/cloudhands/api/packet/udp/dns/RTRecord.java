package com.antell.cloudhands.api.packet.udp.dns;

/**
 * Route Through Record - lists a route preference and intermediate host.
 */

public class RTRecord extends U16NameBase {

    public RTRecord() {
    }

    @Override
    public Record getObject() {
        return new RTRecord();
    }


    /**
     * Gets the preference of the route.
     */
    public int getPreference() {
        return getU16Field();
    }

    /**
     * Gets the host to use as a router.
     */
    public Name getIntermediateHost() {
        return getNameField();
    }

}
