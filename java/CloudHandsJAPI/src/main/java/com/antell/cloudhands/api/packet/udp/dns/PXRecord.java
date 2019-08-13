package com.antell.cloudhands.api.packet.udp.dns;

import com.antell.cloudhands.api.utils.MessagePackUtil;
import org.elasticsearch.common.xcontent.XContentBuilder;
import org.msgpack.core.MessageUnpacker;

import java.io.DataInput;
import java.io.IOException;

/**
 * X.400 mail mapping record.
 */

public class PXRecord extends Record {

    private int preference;
    private Name map822;
    private Name mapX400;

    public PXRecord() {
    }

    @Override
    public Record getObject() {
        return new PXRecord();
    }

    @Override
    public void read(DataInput in) throws IOException {
        preference = in.readUnsignedShort();
        map822 = new Name(in);
        mapX400 = new Name(in);
    }

    @Override
    public void read(MessageUnpacker unpacker) throws IOException {

        MessagePackUtil.parseMapHeader(unpacker,true);
        preference = MessagePackUtil.parseInt(unpacker);
        map822 = new Name(unpacker);
        mapX400 = new Name(unpacker);

    }

    /**
     * Converts the PX Record to a String
     */
    @Override
    public String rrToString() {
        StringBuffer sb = new StringBuffer();
        sb.append(preference);
        sb.append(" ");
        sb.append(map822);
        sb.append(" ");
        sb.append(mapX400);
        return sb.toString();
    }

    @Override
    public XContentBuilder rdataToJson(XContentBuilder cb) throws IOException {

        cb.field("preference",preference);
        cb.field("map822",map822);
        cb.field("mapX400",mapX400);
        return cb;
    }


    /**
     * Gets the preference of the route.
     */
    public int getPreference() {
        return preference;
    }

    /**
     * Gets the RFC 822 component of the mail address.
     */
    public Name getMap822() {
        return map822;
    }

    /**
     * Gets the X.400 component of the mail address.
     */
    public Name getMapX400() {
        return mapX400;
    }

}
