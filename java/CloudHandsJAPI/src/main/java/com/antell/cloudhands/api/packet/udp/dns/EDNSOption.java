package com.antell.cloudhands.api.packet.udp.dns;


import java.io.DataInput;
import java.io.IOException;

/**
 * DNS extension options, as described in RFC 2671.  The rdata of an OPT record
 * is defined as a list of options; this represents a single option.
 */
public abstract class EDNSOption {

    public static class Code {
        private Code() {
        }

        /**
         * Name Server Identifier, RFC 5001
         */
        public final static int NSID = 3;

        /**
         * Client Subnet, defined in draft-vandergaast-edns-client-subnet-02
         */
        public final static int CLIENT_SUBNET = 8;

        private static Mnemonic codes = new Mnemonic("EDNS Option Codes",
                Mnemonic.CASE_UPPER);

        static {
            codes.setMaximum(0xFFFF);
            codes.setPrefix("CODE");
            codes.setNumericAllowed(true);

            codes.add(NSID, "NSID");
            codes.add(CLIENT_SUBNET, "CLIENT_SUBNET");
        }

        /**
         * Converts an EDNS Option Code into its textual representation
         */
        public static String
        string(int code) {
            return codes.getText(code);
        }

        /**
         * Converts a textual representation of an EDNS Option Code into its
         * numeric value.
         *
         * @param s The textual representation of the option code
         * @return The option code, or -1 on error.
         */
        public static int
        value(String s) {
            return codes.getValue(s);
        }
    }

    private final int code;

    /**
     * Creates an option with the given option code and data.
     */
    public EDNSOption(int code) {
        this.code = Record.checkU16("code", code);
    }

    @Override
    public String toString() {
        StringBuffer sb = new StringBuffer();

        sb.append("{");
        sb.append(EDNSOption.Code.string(code));
        sb.append(": ");
        sb.append(optionToString());
        sb.append("}");

        return sb.toString();
    }

    /**
     * Returns the EDNS Option's code.
     *
     * @return the option code
     */
    public int getCode() {
        return code;
    }

    /**
     * Converts the wire format of an EDNS Option (the option data only) into the
     * type-specific format.
     *
     * @param in The input Stream.
     */
    public abstract void read(DataInput in) throws IOException;


   public abstract String optionToString();

}
