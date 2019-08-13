package com.antell.cloudhands.api.packet.udp.dns;

import com.antell.cloudhands.api.utils.Base16;
import com.antell.cloudhands.api.utils.MessagePackUtil;
import com.antell.cloudhands.api.utils.Text;
import org.elasticsearch.common.xcontent.XContentBuilder;
import org.msgpack.core.MessageUnpacker;

import java.io.DataInput;
import java.io.IOException;

/**
 * SSH Fingerprint - stores the fingerprint of an SSH host key.
 */

public class SSHFPRecord extends Record {

    public static class Algorithm {
        private Algorithm() {
        }

        public static final int RSA = 1;
        public static final int DSS = 2;
    }

    public static class Digest {
        private Digest() {
        }

        public static final int SHA1 = 1;
    }

    private int alg;
    private int digestType;
    private byte[] fingerprint;

    public SSHFPRecord() {
    }

    @Override
    public Record getObject() {
        return new SSHFPRecord();
    }


    @Override
    public void read(DataInput in) throws IOException {
        alg = in.readUnsignedByte();
        digestType = in.readUnsignedByte();
        fingerprint = Text.readBytes(in,2);
    }

    @Override
    public void read(MessageUnpacker unpacker) throws IOException {

        MessagePackUtil.parseMapHeader(unpacker,true);
        alg = MessagePackUtil.parseInt(unpacker);
        digestType = MessagePackUtil.parseInt(unpacker);
        fingerprint = MessagePackUtil.parseBin(unpacker);
    }


    @Override
    public String rrToString() {
        StringBuffer sb = new StringBuffer();
        sb.append(alg);
        sb.append(" ");
        sb.append(digestType);
        sb.append(" ");
        sb.append(Base16.toString(fingerprint));
        return sb.toString();
    }

    @Override
    public XContentBuilder rdataToJson(XContentBuilder cb) throws IOException {

        cb.field("alg",alg);
        cb.field("digestType",digestType);
        cb.field("fingerprint",Base16.toString(fingerprint));

        return cb;
    }

    /**
     * Returns the public key's algorithm.
     */
    public int
    getAlgorithm() {
        return alg;
    }

    /**
     * Returns the public key's digest type.
     */
    public int
    getDigestType() {
        return digestType;
    }

    /**
     * Returns the fingerprint
     */
    public byte[]
    getFingerPrint() {
        return fingerprint;
    }


}
