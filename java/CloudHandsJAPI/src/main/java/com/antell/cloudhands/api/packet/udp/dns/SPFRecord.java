package com.antell.cloudhands.api.packet.udp.dns;


/**
 * Sender Policy Framework (RFC 4408, experimental)
 */

public class SPFRecord extends TXTBase {


    public SPFRecord() {
    }

    @Override
    public Record getObject() {
        return new SPFRecord();
    }

}
