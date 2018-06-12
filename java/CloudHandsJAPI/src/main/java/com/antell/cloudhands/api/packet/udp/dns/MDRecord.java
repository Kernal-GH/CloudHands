package com.antell.cloudhands.api.packet.udp.dns;

/**
 * Mail Destination Record  - specifies a mail agent which delivers mail
 * for a domain (obsolete)
 */

public class MDRecord extends SingleNameBase {

    private static final long serialVersionUID = 5268878603762942202L;

    public MDRecord() {
    }

    @Override
    public Record getObject() {
        return new MDRecord();
    }

    /**
     * Creates a new MD Record with the given data
     *
     * @param mailAgent The mail agent that delivers mail for the domain.
     */
    public MDRecord(Name name, int dclass, long ttl, Name mailAgent) {
        super(name, Type.MD, dclass, ttl, mailAgent, "mail agent");
    }

    /**
     * Gets the mail agent for the domain
     */
    public Name getMailAgent() {
        return getSingleName();
    }

    @Override
    public Name getAdditionalName() {
        return getSingleName();
    }

}
