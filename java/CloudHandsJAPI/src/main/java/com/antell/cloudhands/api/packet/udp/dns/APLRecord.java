package com.antell.cloudhands.api.packet.udp.dns;

import com.antell.cloudhands.api.utils.Base16;
import com.antell.cloudhands.api.utils.MessagePackUtil;
import com.antell.cloudhands.api.utils.Text;
import org.elasticsearch.common.xcontent.XContentBuilder;
import org.msgpack.core.MessageUnpacker;

import java.io.DataInput;
import java.io.IOException;
import java.net.InetAddress;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

/**
 * APL - Address Prefix List.  See RFC 3123.
 */

public class APLRecord extends Record {

    public static class Element {
        public final int family;
        public final boolean negative;
        public final int prefixLength;
        public final Object address;

        private Element(int family, boolean negative, Object address, int prefixLength) {
            this.family = family;
            this.negative = negative;
            this.address = address;
            this.prefixLength = prefixLength;
            if (!validatePrefixLength(family, prefixLength)) {
                throw new IllegalArgumentException("invalid prefix " +
                        "length");
            }
        }

        /**
         * Creates an APL element corresponding to an IPv4 or IPv6 prefix.
         *
         * @param negative     Indicates if this prefix is a negation.
         * @param address      The IPv4 or IPv6 address.
         * @param prefixLength The length of this prefix, in bits.
         * @throws IllegalArgumentException The prefix length is invalid.
         */
        public Element(boolean negative, InetAddress address, int prefixLength) {
            this(Address.familyOf(address), negative, address,
                    prefixLength);
        }

        @Override
        public String toString() {
            StringBuffer sb = new StringBuffer();
            if (negative)
                sb.append("!");
            sb.append(family);
            sb.append(":");
            if (family == Address.IPv4 || family == Address.IPv6)
                sb.append(((InetAddress) address).getHostAddress());
            else
                sb.append(Base16.toString((byte[]) address));
            sb.append("/");
            sb.append(prefixLength);
            return sb.toString();
        }

        @Override
        public boolean equals(Object arg) {
            if (arg == null || !(arg instanceof Element))
                return false;
            Element elt = (Element) arg;
            return (family == elt.family &&
                    negative == elt.negative &&
                    prefixLength == elt.prefixLength &&
                    address.equals(elt.address));
        }

        @Override
        public int hashCode() {
            return address.hashCode() + prefixLength + (negative ? 1 : 0);
        }
    }

    private List elements;

    public APLRecord() {
    }

    @Override
    public Record getObject() {
        return new APLRecord();
    }

    private static boolean validatePrefixLength(int family, int prefixLength) {
        if (prefixLength < 0 || prefixLength >= 256)
            return false;
        if ((family == Address.IPv4 && prefixLength > 32) ||
                (family == Address.IPv6 && prefixLength > 128))
            return false;
        return true;
    }

    private static byte[] parseAddress(byte[] in, int length) throws ParseException {
        if (in.length > length)
            throw new ParseException("invalid address length");
        if (in.length == length)
            return in;
        byte[] out = new byte[length];
        System.arraycopy(in, 0, out, 0, in.length);
        return out;
    }

    @Override
    public void read(DataInput in) throws IOException {
        elements = new ArrayList(1);
        int n = in.readUnsignedShort();

        for(int i = 0;i<n;i++) {

            int family = in.readUnsignedShort();
            int prefix = in.readUnsignedByte();
            boolean negative = in.readUnsignedByte()==1;

            byte[] data = Text.readBytes(in,2);

            Element element;
            if (!validatePrefixLength(family, prefix)) {
                throw new ParseException("invalid prefix length");
            }

            if (family == Address.IPv4 || family == Address.IPv6) {
                data = parseAddress(data,
                        Address.addressLength(family));
                InetAddress addr = InetAddress.getByAddress(data);
                element = new Element(negative, addr, prefix);
            } else {
                element = new Element(family, negative, data, prefix);
            }
            elements.add(element);

        }
    }

    @Override
    public void read(MessageUnpacker unpacker) throws IOException {

        MessagePackUtil.parseMapHeader(unpacker,true);
        int n = MessagePackUtil.parseArrayHeader(unpacker,true);

        for(int i = 0;i<n;i++) {

            MessagePackUtil.parseMapHeader(unpacker,false);

            int family = MessagePackUtil.parseInt(unpacker);
            int prefix = MessagePackUtil.parseByte(unpacker);
            boolean negative = MessagePackUtil.parseByte(unpacker)==1;

            byte[] data = MessagePackUtil.parseBin(unpacker);

            Element element;
            if (!validatePrefixLength(family, prefix)) {
                throw new ParseException("invalid prefix length");
            }

            if (family == Address.IPv4 || family == Address.IPv6) {
                data = parseAddress(data,
                        Address.addressLength(family));
                InetAddress addr = InetAddress.getByAddress(data);
                element = new Element(negative, addr, prefix);
            } else {
                element = new Element(family, negative, data, prefix);
            }
            elements.add(element);

        }
    }

    @Override
    public String rrToString() {
        StringBuffer sb = new StringBuffer();
        for (Iterator it = elements.iterator(); it.hasNext(); ) {
            Element element = (Element) it.next();
            sb.append(element);
            if (it.hasNext())
                sb.append(" ");
        }
        return sb.toString();
    }

    @Override
    XContentBuilder rdataToJson(XContentBuilder cb) throws IOException {

        cb.field("APLRecords",elements);
        return cb;

    }

    /**
     * Returns the list of APL elements.
     */
    public List getElements() {
        return elements;
    }

    private static int addressLength(byte[] addr) {
        for (int i = addr.length - 1; i >= 0; i--) {
            if (addr[i] != 0)
                return i + 1;
        }
        return 0;
    }


}
