package com.antell.cloudhands.api.packet.udp.dns;

/**
 * An exception thrown when an invalid type code is specified.
 *
 */

public class InvalidTypeException extends IllegalArgumentException {

    public InvalidTypeException(int type) {
        super("Invalid DNS type: " + type);
    }

}
