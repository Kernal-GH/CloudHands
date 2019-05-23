package com.antell.cloudhands.api.packet.udp.dns;

/**
 * An exception thrown when an invalid TTL is specified.
 *
 */

public class InvalidTTLException extends IllegalArgumentException {

    public InvalidTTLException(long ttl) {
        super("Invalid DNS TTL: " + ttl);
    }

}
