package com.antell.cloudhands.api.packet.udp.dns;

/**
 * Implements common functionality for the many record types whose format
 * is a single compressed name.
 */

public abstract class SingleCompressedNameBase extends SingleNameBase {


    protected SingleCompressedNameBase() {
    }

    protected SingleCompressedNameBase(Name name, int type, int dclass, long ttl,
                                       Name singleName, String description) {
        super(name, type, dclass, ttl, singleName, description);
    }

}
