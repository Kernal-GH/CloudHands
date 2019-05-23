package com.antell.cloudhands.api.packet.udp.dns;


import java.io.IOException;

/**
 * An exception thrown when a DNS message is invalid.
 *
 */

public class ParseException extends IOException {

    public ParseException() {
        super();
    }

    public ParseException(String s) {
        super(s);
    }

    public ParseException(String s, Throwable cause) {
        super(s);
        initCause(cause);
    }

}