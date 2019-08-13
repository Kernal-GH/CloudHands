package com.antell.cloudhands.api.packet.udp.dns;

import org.elasticsearch.common.xcontent.XContentBuilder;
import org.msgpack.core.MessageUnpacker;

import java.io.DataInput;
import java.io.IOException;
import java.net.InetAddress;

/**
 * A6 Record - maps a domain name to an IPv6 address (experimental)
 *
 */

public class A6Record extends Record {

    private int prefixBits;
    private InetAddress suffix;
    private Name prefix;

    public A6Record() {
    }

    @Override
    public Record getObject() {
        return new A6Record();
    }


    @Override
    public void read(DataInput in) throws IOException {
        prefixBits = in.readUnsignedByte();
        int suffixbits = 128 - prefixBits;
        int suffixbytes = (suffixbits + 7) / 8;
        if (prefixBits < 128) {
            byte[] bytes = new byte[16];
            in.readFully(bytes, 16 - suffixbytes, suffixbytes);
            suffix = InetAddress.getByAddress(bytes);
        }
        if (prefixBits > 0)
            prefix = new Name(in);
    }

    @Override
    public void read(MessageUnpacker unpacker) throws IOException {

    }

    /**
     * Converts rdata to a String
     */
    @Override
    public String rrToString() {
        StringBuffer sb = new StringBuffer();
        sb.append(prefixBits);
        if (suffix != null) {
            sb.append(" ");
            sb.append(suffix.getHostAddress());
        }
        if (prefix != null) {
            sb.append(" ");
            sb.append(prefix);
        }
        return sb.toString();
    }

    @Override
    XContentBuilder rdataToJson(XContentBuilder cb) throws IOException {

        cb.field("prefixBits",prefixBits);
        cb.field("suffix",suffix==null?"":suffix.getHostAddress());
        cb.field("prefix",prefix==null?"":prefix.toString());

        return cb;
    }

    /**
     * Returns the number of bits in the prefix
     */
    public int getPrefixBits() {
        return prefixBits;
    }

    /**
     * Returns the address suffix
     */
    public InetAddress getSuffix() {
        return suffix;
    }

    /**
     * Returns the address prefix
     */
    public Name getPrefix() {
        return prefix;
    }


}
