package com.antell.cloudhands.api.packet.udp.dns;

import com.antell.cloudhands.api.utils.MessagePackUtil;
import com.antell.cloudhands.api.utils.Text;
import org.elasticsearch.common.xcontent.XContentBuilder;
import org.msgpack.core.MessageUnpacker;

import java.io.DataInput;
import java.io.IOException;

/**
 * ISDN - identifies the ISDN number and subaddress associated with a name.
 */

public class ISDNRecord extends Record {


    private String address;
    private String subAddress;

    public ISDNRecord() {
    }

    @Override
    public Record getObject() {
        return new ISDNRecord();
    }

    @Override
    public void read(DataInput in) throws IOException {
        address = Text.readString(in,2);

        subAddress = Text.readString(in,2);

    }

    @Override
    public void read(MessageUnpacker unpacker) throws IOException {

        MessagePackUtil.parseMapHeader(unpacker,true);
        address = MessagePackUtil.parseText(unpacker);
        subAddress = MessagePackUtil.parseText(unpacker);

    }

    /**
     * Returns the ISDN number associated with the domain.
     */
    public String
    getAddress() {
        return address;
    }

    /**
     * Returns the ISDN subaddress, or null if there is none.
     */
    public String
    getSubAddress() {

        return subAddress;
    }

    @Override
    public String rrToString() {
        StringBuffer sb = new StringBuffer();
        sb.append(address);
        if (subAddress != null) {
            sb.append(" ");
            sb.append(subAddress);
        }
        return sb.toString();
    }

    @Override
    public XContentBuilder rdataToJson(XContentBuilder cb) throws IOException {

        cb.field("address",address);
        cb.field("subAddress",subAddress==null?"":subAddress);

        return cb;
    }

}
