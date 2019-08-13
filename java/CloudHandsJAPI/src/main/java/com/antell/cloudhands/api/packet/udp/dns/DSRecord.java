package com.antell.cloudhands.api.packet.udp.dns;

import com.antell.cloudhands.api.utils.Base16;
import com.antell.cloudhands.api.utils.MessagePackUtil;
import com.antell.cloudhands.api.utils.Text;
import org.elasticsearch.common.xcontent.XContentBuilder;
import org.msgpack.core.MessageUnpacker;

import java.io.DataInput;
import java.io.IOException;

/**
 * DS - contains a Delegation Signer record, which acts as a
 * placeholder for KEY records in the parent zone.
 *
 */

public class DSRecord extends Record {

    public static class Digest {
        private Digest() {
        }

        /**
         * SHA-1
         */
        public static final int SHA1 = 1;

        /**
         * SHA-256
         */
        public static final int SHA256 = 2;

        /**
         * GOST R 34.11-94
         */
        public static final int GOST3411 = 3;

        /**
         * SHA-384
         */
        public static final int SHA384 = 4;
    }

    public static final int SHA1_DIGEST_ID = Digest.SHA1;
    public static final int SHA256_DIGEST_ID = Digest.SHA256;
    public static final int GOST3411_DIGEST_ID = Digest.GOST3411;
    public static final int SHA384_DIGEST_ID = Digest.SHA384;

    private int footprint;
    private int alg;
    private int digestid;
    private byte[] digest;

    public DSRecord() {
    }

    @Override
    public Record getObject() {
        return new DSRecord();
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
        cb.field("digest",digest == null?"":Base16.toString(digest));

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
