package com.antell.cloudhands.api.packet.udp.dns;

/**
 * Mail Group Record  - specifies a mailbox which is a member of a mail group.
 *
 */

public class MGRecord extends SingleNameBase {


    public MGRecord() {
    }

    @Override
    public Record getObject() {
        return new MGRecord();
    }

    /**
     * Gets the mailbox in the mail group specified by the domain
     */
    public Name getMailbox() {
        return getSingleName();
    }

}
