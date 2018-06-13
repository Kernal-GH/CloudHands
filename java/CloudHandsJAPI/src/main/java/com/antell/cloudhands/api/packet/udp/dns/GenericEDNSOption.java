package com.antell.cloudhands.api.packet.udp.dns;


import com.antell.cloudhands.api.utils.Base16;
import com.antell.cloudhands.api.utils.Text;

import java.io.DataInput;
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
    public void read(DataInput in) throws IOException {
        data = Text.readBytes(in,2);
    }


    @Override
    public String optionToString() {
        return "<" + Base16.toString(data) + ">";
    }

}
