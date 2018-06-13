package com.antell.cloudhands.api.packet.udp.dns;

/**
 * Mail Forwarder Record  - specifies a mail agent which forwards mail
 * for a domain (obsolete)
 */

public class MFRecord extends SingleNameBase {


    public MFRecord() {
    }

    @Override
    public Record getObject() {
        return new MFRecord();
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
