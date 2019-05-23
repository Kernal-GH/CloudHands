package com.antell.cloudhands.api.packet.udp.dns;

import java.io.IOException;

/**
 * An exception thrown when unable to parse text.
 *
 */

public class TextParseException extends IOException {

    public TextParseException() {
        super();
    }

    public TextParseException(String s) {
        super(s);
    }

}
