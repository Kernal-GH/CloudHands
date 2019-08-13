package com.antell.cloudhands.api.packet.udp.dns;

import com.antell.cloudhands.api.utils.MessagePackUtil;
import com.google.common.base.Preconditions;
import org.elasticsearch.common.xcontent.XContentBuilder;
import org.msgpack.core.MessageUnpacker;

import java.io.DataInput;
import java.io.IOException;

/**
 * A DNS  header
 */

public class Header {

    private int id;
    private int flags;
    private int[] counts;

    /**
     * Parses a Header from a stream containing DNS wire format.
     */
    public Header(DataInput in) throws IOException {

        id = in.readUnsignedShort();
        flags = in.readUnsignedShort();
        counts = new int[4];

        for (int i = 0; i < counts.length; i++)
            counts[i] = in.readUnsignedShort();


    }

    public Header(MessageUnpacker unpacker) throws IOException {

        int n = MessagePackUtil.parseMapHeader(unpacker,true);
        Preconditions.checkArgument(n==6,"Invalid msgpack packet of udp session header entry:"+n);

        id = MessagePackUtil.parseInt(unpacker);
        flags = MessagePackUtil.parseInt(unpacker);
        counts = new int[4];
        for (int i = 0; i < counts.length; i++)
            counts[i] = MessagePackUtil.parseInt(unpacker);

    }

    static private boolean validFlag(int bit) {
        return (bit >= 0 && bit <= 0xF && Flags.isFlag(bit));
    }

    static private void
    checkFlag(int bit) {
        if (!validFlag(bit))
            throw new IllegalArgumentException("invalid flag bit " + bit);
    }

    /**
     * Retrieves a flag
     *
     * @see Flags
     */
    public boolean
    getFlag(int bit) {
        checkFlag(bit);
        // bits are indexed from left to right
        return (flags & (1 << (15 - bit))) != 0;
    }

    boolean[]
    getFlags() {
        boolean[] array = new boolean[16];
        for (int i = 0; i < array.length; i++)
            if (validFlag(i))
                array[i] = getFlag(i);
        return array;
    }

    /**
     * Retrieves the message ID
     */
    public int
    getID() {
        return id;
    }

    /**
     * Retrieves the mesasge's rcode
     *
     * @see Rcode
     */
    public int
    getRcode() {
        return flags & 0xF;
    }

    /**
     * Retrieves the mesasge's opcode
     *
     * @see Opcode
     */
    public int
    getOpcode() {
        return (flags >> 11) & 0xF;
    }

    /**
     * Retrieves the record count for the given section
     *
     * @see Section
     */
    public int
    getCount(int field) {
        return counts[field];
    }

    public int getFlagsByte() {
        return flags;
    }

    /**
     * Converts the header's flags into a String
     */
    public String
    printFlags() {
        StringBuffer sb = new StringBuffer();

        for (int i = 0; i < 16; i++)
            if (validFlag(i) && getFlag(i)) {
                sb.append(Flags.string(i));
                sb.append(" ");
            }
        return sb.toString();
    }

    public String
    toStringWithRcode(int newrcode) {
        StringBuffer sb = new StringBuffer();

        sb.append(";; ->>HEADER<<- ");
        sb.append("opcode: " + Opcode.string(getOpcode()));
        sb.append(", status: " + Rcode.string(newrcode));
        sb.append(", id: " + getID());
        sb.append("\n");

        sb.append(";; flags: " + printFlags());
        sb.append("; ");
        for (int i = 0; i < 4; i++)
            sb.append(Section.string(i) + ": " + getCount(i) + " ");
        return sb.toString();
    }

    /**
     * Converts the header into a String
     */
    public String
    toString() {
        return toStringWithRcode(getRcode());
    }


    public XContentBuilder toJson(XContentBuilder cb) throws IOException {

        cb.field("opcode", Opcode.string(getOpcode()));
        cb.field("status",Rcode.string(getRcode()));
        cb.field("id",getID());
        cb.field("flags",printFlags());

        for(int i =0;i<4;i++){
            cb.field(Section.string(i),getCount(i));
        }

        return cb;
    }
}
