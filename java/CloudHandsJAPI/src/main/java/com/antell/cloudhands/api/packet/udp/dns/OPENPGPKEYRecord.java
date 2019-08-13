package com.antell.cloudhands.api.packet.udp.dns;

import com.antell.cloudhands.api.utils.Base64;
import com.antell.cloudhands.api.utils.MessagePackUtil;
import com.antell.cloudhands.api.utils.Text;
import org.elasticsearch.common.xcontent.XContentBuilder;
import org.msgpack.core.MessageUnpacker;

import java.io.DataInput;
import java.io.IOException;

/**
 * OPENPGPKEY Record - Stores an OpenPGP certificate associated with a name.
 * RFC 7929.
 */
public class OPENPGPKEYRecord extends Record {

    private byte[] cert;

    public OPENPGPKEYRecord() {
    }

    @Override
    public Record getObject() {
        return new OPENPGPKEYRecord();
    }


    @Override
    public void read(DataInput in) throws IOException {
        cert = Text.readBytes(in,2);
    }

    @Override
    public void read(MessageUnpacker unpacker) throws IOException {

        MessagePackUtil.parseMapHeader(unpacker,true);
        cert = MessagePackUtil.parseBin(unpacker);
    }


    /**
     * Converts rdata to a String
     */
    @Override
    public String rrToString() {
        StringBuffer sb = new StringBuffer();
        if (cert != null) {
            sb.append(Base64.toString(cert));
        }
        return sb.toString();
    }

    @Override
    public XContentBuilder rdataToJson(XContentBuilder cb) throws IOException {

        cb.field("cert",cert==null?"":Base64.toString(cert));

        return cb;
    }

    /**
     * Returns the binary representation of the certificate
     */
    public byte[]
    getCert() {
        return cert;
    }


}
