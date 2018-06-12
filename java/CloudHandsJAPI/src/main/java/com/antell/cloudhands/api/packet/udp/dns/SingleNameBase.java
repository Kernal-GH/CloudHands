package com.antell.cloudhands.api.packet.udp.dns;

import java.io.DataInput;
import java.io.IOException;

/**
 * Implements common functionality for the many record types whose format
 * is a single name.
 *
 */

abstract class SingleNameBase extends Record {


    protected Name singleName;

    protected SingleNameBase() {
    }

    protected SingleNameBase(Name name, int type, int dclass, long ttl) {
        super(name, type, dclass, ttl);
    }

    protected SingleNameBase(Name name, int type, int dclass, long ttl, Name singleName,
                             String description) {
        super(name, type, dclass, ttl);
        this.singleName = singleName;
    }

    @Override
    public void read(DataInput in) throws IOException {
        singleName = new Name(in);
    }


    @Override
    public String rrToString() {
        return singleName.toString();
    }

    protected Name getSingleName() {
        return singleName;
    }


}
