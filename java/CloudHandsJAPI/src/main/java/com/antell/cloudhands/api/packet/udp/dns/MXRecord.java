package com.antell.cloudhands.api.packet.udp.dns;

/**
 * Mail Exchange - specifies where mail to a domain is sent
 */

public class MXRecord extends U16NameBase {

    public MXRecord() {
    }

    @Override
    public Record getObject() {
        return new MXRecord();
    }

    /**
     * Returns the target of the MX record
     */
    public Name getTarget() {
        return getNameField();
    }

    /**
     * Returns the priority of this MX record
     */
    public int
    getPriority() {
        return getU16Field();
    }

    @Override
    public Name getAdditionalName() {
        return getNameField();
    }

}
