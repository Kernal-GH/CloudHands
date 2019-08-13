package com.antell.cloudhands.api.packet.udp.dns;

import com.antell.cloudhands.api.utils.Base64;
import com.antell.cloudhands.api.utils.MessagePackUtil;
import com.antell.cloudhands.api.utils.Text;
import org.elasticsearch.common.xcontent.XContentBuilder;
import org.msgpack.core.MessageUnpacker;

import java.io.DataInput;
import java.io.IOException;

/**
 * DHCID - Dynamic Host Configuration Protocol (DHCP) ID (RFC 4701)
 *
 */

public class DHCIDRecord extends Record {

    private byte[] data;

    public DHCIDRecord() {
    }

    @Override
    public Record getObject() {
        return new DHCIDRecord();
    }

    @Override
    public void read(DataInput in) throws IOException {

        data = Text.readBytes(in,2);

    }

    @Override
    public void read(MessageUnpacker unpacker) throws IOException {

        MessagePackUtil.parseMapHeader(unpacker,true);
        data = MessagePackUtil.parseBin(unpacker);
    }

    @Override
    public String rrToString() {
        return Base64.toString(data);
    }

    @Override
    XContentBuilder rdataToJson(XContentBuilder cb) throws IOException {


        cb.field("data",Base64.toString(data));
        return cb;

    }


    /**
     * Returns the binary data.
     */
    public byte[] getData() {
        return data;
    }

}
