package com.antell.cloudhands.api.packet.udp.dns;

/**
 * Key - contains a cryptographic public key for use by DNS.
 * The data can be converted to objects implementing
 * java.security.interfaces.PublicKey
 *
 */

public class DNSKEYRecord extends KEYBase {

    public static class Protocol {
        private Protocol() {
        }

        /**
         * Key will be used for DNSSEC
         */
        public static final int DNSSEC = 3;
    }

    public static class Flags {
        private Flags() {
        }

        /**
         * Key is a zone key
         */
        public static final int ZONE_KEY = 0x100;

        /**
         * Key is a secure entry point key
         */
        public static final int SEP_KEY = 0x1;

        /**
         * Key has been revoked
         */
        public static final int REVOKE = 0x80;
    }

    public DNSKEYRecord() {
    }

    @Override
    public Record getObject() {
        return new DNSKEYRecord();
    }


}
