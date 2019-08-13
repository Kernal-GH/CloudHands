package com.antell.cloudhands.api.packet.udp.dns;

import com.antell.cloudhands.api.utils.Base16;
import com.antell.cloudhands.api.utils.MessagePackUtil;
import com.antell.cloudhands.api.utils.Text;
import org.elasticsearch.common.xcontent.XContentBuilder;
import org.msgpack.core.MessageUnpacker;

import java.io.DataInput;
import java.io.IOException;
import java.security.NoSuchAlgorithmException;

/**
 * Next SECure name 3 Parameters - this record contains the parameters (hash
 * algorithm, salt, iterations) used for a valid, complete NSEC3 chain present
 * in a zone. Zones signed using NSEC3 must include this record at the zone apex
 * to inform authoritative servers that NSEC3 is being used with the given
 * parameters.
 */

public class NSEC3PARAMRecord extends Record {

    private int hashAlg;
    private int flags;
    private int iterations;
    private byte salt[];

    public NSEC3PARAMRecord() {
    }

    @Override
    public Record getObject() {
        return new NSEC3PARAMRecord();
    }

    @Override
    public void read(DataInput in) throws IOException {
        hashAlg = in.readUnsignedByte();
        flags = in.readUnsignedByte();
        iterations = in.readUnsignedShort();

        salt = Text.readBytes(in,2);
    }

    @Override
    public void read(MessageUnpacker unpacker) throws IOException {

        MessagePackUtil.parseMapHeader(unpacker,true);
        hashAlg = MessagePackUtil.parseInt(unpacker);
        flags = MessagePackUtil.parseInt(unpacker);
        iterations = MessagePackUtil.parseInt(unpacker);
        salt = MessagePackUtil.parseBin(unpacker);
    }

    /**
     * Converts rdata to a String
     */
    @Override
    public String rrToString() {
        StringBuffer sb = new StringBuffer();
        sb.append(hashAlg);
        sb.append(' ');
        sb.append(flags);
        sb.append(' ');
        sb.append(iterations);
        sb.append(' ');
        if (salt == null)
            sb.append('-');
        else
            sb.append(Base16.toString(salt));

        return sb.toString();
    }

    @Override
    public XContentBuilder rdataToJson(XContentBuilder cb) throws IOException {

        cb.field("hashAlg",hashAlg);
        cb.field("flags",flags);
        cb.field("iterations",iterations);
        cb.field("salt",salt == null?"":Base16.toString(salt));

        return cb;
    }

    /**
     * Returns the hash algorithm
     */
    public int getHashAlgorithm() {
        return hashAlg;
    }

    /**
     * Returns the flags
     */
    public int getFlags() {
        return flags;
    }

    /**
     * Returns the number of iterations
     */
    public int getIterations() {
        return iterations;
    }

    /**
     * Returns the salt
     */
    public byte[] getSalt() {
        return salt;
    }


}
