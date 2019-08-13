
package com.antell.cloudhands.api.packet.udp.dns;


import com.antell.cloudhands.api.utils.Base16;
import com.antell.cloudhands.api.utils.MessagePackUtil;
import com.antell.cloudhands.api.utils.Text;
import org.elasticsearch.common.xcontent.XContentBuilder;
import org.msgpack.core.MessageUnpacker;

import java.io.DataInput;
import java.io.IOException;


/**
 * DLV - contains a Delegation Lookaside Validation record, which acts
 * as the equivalent of a DS record in a lookaside zone.
 *
 * @see DSRecord
 */

public class DLVRecord extends Record {

    public static final int SHA1_DIGEST_ID = DSRecord.Digest.SHA1;
    public static final int SHA256_DIGEST_ID = DSRecord.Digest.SHA1;

    private int footprint;
    private int alg;
    private int digestid;
    private byte[] digest;

    public DLVRecord() {
    }

    @Override
    public Record getObject() {
        return new DLVRecord();
    }

    @Override
    public void read(DataInput in) throws IOException {
        footprint = in.readUnsignedShort();
        alg = in.readUnsignedByte();
        digestid = in.readUnsignedByte();
        digest = Text.readBytes(in,2);

    }

    @Override
    public void read(MessageUnpacker unpacker) throws IOException {

        MessagePackUtil.parseMapHeader(unpacker,true);
        footprint = MessagePackUtil.parseInt(unpacker);
        alg = MessagePackUtil.parseInt(unpacker);
        digestid = MessagePackUtil.parseInt(unpacker);
        digest = MessagePackUtil.parseBin(unpacker);

    }

    /**
     * Converts rdata to a String
     */
    @Override
    public String rrToString() {
        StringBuffer sb = new StringBuffer();
        sb.append(footprint);
        sb.append(" ");
        sb.append(alg);
        sb.append(" ");
        sb.append(digestid);
        if (digest != null) {
            sb.append(" ");
            sb.append(Base16.toString(digest));
        }

        return sb.toString();
    }

    @Override
    XContentBuilder rdataToJson(XContentBuilder cb) throws IOException {
        cb.field("footprint",footprint);
        cb.field("alg",alg);
        cb.field("digestid",digestid);
        cb.field("digest",digest==null?"":Base16.toString(digest));
        return cb;
    }

    /**
     * Returns the key's algorithm.
     */
    public int
    getAlgorithm() {
        return alg;
    }

    /**
     * Returns the key's Digest ID.
     */
    public int
    getDigestID() {
        return digestid;
    }

    /**
     * Returns the binary hash of the key.
     */
    public byte[]
    getDigest() {
        return digest;
    }

    /**
     * Returns the key's footprint.
     */
    public int
    getFootprint() {
        return footprint;
    }


}
