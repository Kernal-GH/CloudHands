package com.antell.cloudhands.api.packet.udp.dns;

import com.antell.cloudhands.api.utils.MessagePackUtil;
import com.google.common.io.ByteStreams;
import org.elasticsearch.common.xcontent.XContentBuilder;
import org.msgpack.core.MessageUnpacker;

import java.io.DataInput;
import java.io.IOException;
import java.util.BitSet;

/**
 * Next name - this record contains the following name in an ordered list
 * of names in the zone, and a set of types for which records exist for
 * this name.  The presence of this record in a response signifies a
 * failed query for data in a DNSSEC-signed zone.
 */

public class NXTRecord extends Record {

    private Name next;
    private BitSet bitmap;

    public NXTRecord() {
    }

    @Override
    public Record getObject() {
        return new NXTRecord();
    }


    @Override
    public void read(DataInput in) throws IOException {
        next = new Name(in);
        bitmap = new BitSet();

        int bitmapLength = in.readUnsignedShort();
        for (int i = 0; i < bitmapLength; i++) {
            int t = in.readUnsignedByte();
            for (int j = 0; j < 8; j++)
                if ((t & (1 << (7 - j))) != 0)
                    bitmap.set(i * 8 + j);
        }
    }

    @Override
    public void read(MessageUnpacker unpacker) throws IOException {

        MessagePackUtil.parseMapHeader(unpacker,true);
        next = new Name(unpacker);
        bitmap = new BitSet();
        DataInput in = ByteStreams.newDataInput(MessagePackUtil.parseBin(unpacker));
        int bitmapLength = in.readUnsignedShort();
        for (int i = 0; i < bitmapLength; i++) {
            int t = in.readUnsignedByte();
            for (int j = 0; j < 8; j++)
                if ((t & (1 << (7 - j))) != 0)
                    bitmap.set(i * 8 + j);
        }
    }


    /**
     * Converts rdata to a String
     */
    @Override
    public String rrToString() {
        StringBuffer sb = new StringBuffer();
        sb.append(next);
        int length = bitmap.length();
        for (short i = 0; i < length; i++)
            if (bitmap.get(i)) {
                sb.append(" ");
                sb.append(Type.string(i));
            }
        return sb.toString();
    }

    @Override
    public XContentBuilder rdataToJson(XContentBuilder cb) throws IOException {

        StringBuffer sb = new StringBuffer();

        cb.field("next",next);

        int length = bitmap.length();
        for (short i = 0; i < length; i++)
            if (bitmap.get(i)) {
                sb.append(" ");
                sb.append(Type.string(i));
            }

        cb.field("bitmap",sb.toString());
        return cb;
    }

    /**
     * Returns the next name
     */
    public Name getNext() {
        return next;
    }

    /**
     * Returns the set of types defined for this name
     */
    public BitSet getBitmap() {
        return bitmap;
    }
}
