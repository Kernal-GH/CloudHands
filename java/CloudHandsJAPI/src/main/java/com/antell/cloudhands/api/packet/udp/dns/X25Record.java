package com.antell.cloudhands.api.packet.udp.dns;

import com.antell.cloudhands.api.utils.MessagePackUtil;
import com.antell.cloudhands.api.utils.Text;
import org.elasticsearch.common.xcontent.XContentBuilder;
import org.msgpack.core.MessageUnpacker;

import java.io.DataInput;
import java.io.IOException;

/**
 * X25 - identifies the PSDN (Public Switched Data Network) address in the
 * X.121 numbering plan associated with a name.
 *
 */

public class X25Record extends Record {



    private String address;

    public X25Record() {
    }

    @Override
    public Record getObject() {
        return new X25Record();
    }


    @Override
    public void read(DataInput in) throws IOException {
        address = Text.readString(in,2);
    }

    @Override
    public void read(MessageUnpacker unpacker) throws IOException {

        MessagePackUtil.parseMapHeader(unpacker,true);
        address = MessagePackUtil.parseText(unpacker);

    }

    /**
     * Returns the X.25 PSDN address.
     */
    public String getAddress() {

        return address;
    }

    @Override
    public String rrToString() {

        return address;
    }

    @Override
    public XContentBuilder rdataToJson(XContentBuilder cb) throws IOException {

        cb.field("address",address);

        return cb;
    }

}
