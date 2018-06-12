package com.antell.cloudhands.api.packet.udp.dns;

import java.io.DataInput;
import java.io.IOException;

/**
 * Implements common functionality for the many record types whose format
 * is an unsigned 16 bit integer followed by a name.
 */

public abstract class U16NameBase extends Record {

    protected int u16Field;
    protected Name nameField;

    protected U16NameBase() {
    }

    @Override
    public void read(DataInput in) throws IOException {
        u16Field = in.readUnsignedShort();
        nameField = new Name(in);
    }

    @Override
    public String rrToString() {
        StringBuffer sb = new StringBuffer();
        sb.append(u16Field);
        sb.append(" ");
        sb.append(nameField);
        return sb.toString();
    }

    protected int getU16Field() {
        return u16Field;
    }

    protected Name getNameField() {
        return nameField;
    }


}
