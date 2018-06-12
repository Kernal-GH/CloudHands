package com.antell.cloudhands.api.packet.udp.dns;

import com.antell.security.utils.Base16;

import java.io.IOException;

/**
 * An EDNSOption with no internal structure.
 *
 */
public class GenericEDNSOption extends EDNSOption {

    private byte[] data;

    public GenericEDNSOption(int code) {
        super(code);
    }

    /**
     * Construct a generic EDNS option.
     *
     * @param data The contents of the option.
     */
    public GenericEDNSOption(int code, byte[] data) {
        super(code);
        this.data = Record.checkByteArrayLength("option data", data, 0xFFFF);
    }

    @Override
    public void optionFromWire(DNSInput in) throws IOException {
        data = in.readByteArray();
    }

    @Override
    public void optionToWire(DNSOutput out) {
        out.writeByteArray(data);
    }

    @Override
    public String optionToString() {
        return "<" + Base16.toString(data) + ">";
    }

}
