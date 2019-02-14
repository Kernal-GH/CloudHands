
package com.antell.cloudhands.api.packet.udp.dns;

/**
 * An exception thrown when an invalid dclass code is specified.
 *
 */

public class InvalidDClassException extends IllegalArgumentException {

    public InvalidDClassException(int dclass) {
        super("Invalid DNS class: " + dclass);
    }

}
