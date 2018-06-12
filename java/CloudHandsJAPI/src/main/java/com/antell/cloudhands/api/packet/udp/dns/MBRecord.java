package com.antell.cloudhands.api.packet.udp.dns;

/**
 * Mailbox Record  - specifies a host containing a mailbox.
 */

public class MBRecord extends SingleNameBase {

    private static final long serialVersionUID = 532349543479150419L;

    public MBRecord() {
    }

    @Override
    public Record getObject() {
        return new MBRecord();
    }

    /**
     * Creates a new MB Record with the given data
     *
     * @param mailbox The host containing the mailbox for the domain.
     */
    public MBRecord(Name name, int dclass, long ttl, Name mailbox) {
        super(name, Type.MB, dclass, ttl, mailbox, "mailbox");
    }

    /**
     * Gets the mailbox for the domain
     */
    public Name getMailbox() {
        return getSingleName();
    }

    @Override
    public Name getAdditionalName() {
        return getSingleName();
    }

}
