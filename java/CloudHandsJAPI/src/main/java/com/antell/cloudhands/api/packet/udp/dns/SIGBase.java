package com.antell.cloudhands.api.packet.udp.dns;

import com.antell.cloudhands.api.utils.Base64;
import com.antell.cloudhands.api.utils.MessagePackUtil;
import com.antell.cloudhands.api.utils.Text;
import org.elasticsearch.common.xcontent.XContentBuilder;
import org.msgpack.core.MessageUnpacker;

import java.io.DataInput;
import java.io.IOException;
import java.util.Date;

/**
 * The base class for SIG/RRSIG records, which have identical formats
 */

public abstract class SIGBase extends Record {

    protected int covered;
    protected int alg;
    protected int labels;
    protected long origttl;
    protected Date expire;
    protected Date timeSigned;
    protected int footprint;
    protected Name signer;
    protected byte[] signature;

    protected SIGBase() {
    }


    @Override
    public void read(DataInput in) throws IOException {
        covered = in.readUnsignedShort();
        alg = in.readUnsignedByte();
        labels = in.readUnsignedByte();
        origttl = in.readInt();
        expire = new Date(1000 * in.readInt());
        timeSigned = new Date(1000 * in.readInt());
        footprint = in.readUnsignedShort();
        signer = new Name(in);
        signature = Text.readBytes(in,2);
    }

    @Override
    public void read(MessageUnpacker unpacker) throws IOException {

        MessagePackUtil.parseMapHeader(unpacker,true);
        covered = MessagePackUtil.parseInt(unpacker);
        alg = MessagePackUtil.parseInt(unpacker);
        labels = MessagePackUtil.parseInt(unpacker);
        origttl = MessagePackUtil.parseLong(unpacker);
        expire = new Date(1000 * MessagePackUtil.parseLong(unpacker));
        timeSigned = new Date(1000 * MessagePackUtil.parseLong(unpacker));
        footprint = MessagePackUtil.parseInt(unpacker);
        signer = new Name(unpacker);
        signature = MessagePackUtil.parseBin(unpacker);
    }

    /**
     * Converts the RRSIG/SIG Record to a String
     */
    @Override
    public String rrToString() {
        StringBuffer sb = new StringBuffer();
        sb.append(Type.string(covered));
        sb.append(" ");
        sb.append(alg);
        sb.append(" ");
        sb.append(labels);
        sb.append(" ");
        sb.append(origttl);
        sb.append(" ");
        sb.append(FormattedTime.format(expire));
        sb.append(" ");
        sb.append(FormattedTime.format(timeSigned));
        sb.append(" ");
        sb.append(footprint);
        sb.append(" ");
        sb.append(signer);
        sb.append(" ");
        sb.append(Base64.toString(signature));
        return sb.toString();
    }
    @Override
    public XContentBuilder rdataToJson(XContentBuilder cb) throws IOException {

        cb.field("covered",Type.string(covered));
        cb.field("alg",alg);
        cb.field("labels",labels);
        cb.field("origttl",origttl);
        cb.field("expire",FormattedTime.format(expire));
        cb.field("timeSigned",FormattedTime.format(timeSigned));
        cb.field("footprint",footprint);
        cb.field("signer",signer);
        cb.field("signature",Base64.toString(signature));

        return cb;
    }

    /**
     * Returns the RRset type covered by this signature
     */
    public int
    getTypeCovered() {
        return covered;
    }

    /**
     * Returns the cryptographic algorithm of the key that generated the signature
     */
    public int
    getAlgorithm() {
        return alg;
    }

    /**
     * Returns the number of labels in the signed domain name.  This may be
     * different than the record's domain name if the record is a wildcard
     * record.
     */
    public int
    getLabels() {
        return labels;
    }

    /**
     * Returns the original TTL of the RRset
     */
    public long
    getOrigTTL() {
        return origttl;
    }

    /**
     * Returns the time at which the signature expires
     */
    public Date
    getExpire() {
        return expire;
    }

    /**
     * Returns the time at which this signature was generated
     */
    public Date
    getTimeSigned() {
        return timeSigned;
    }

    /**
     * Returns The footprint/key id of the signing key.
     */
    public int
    getFootprint() {
        return footprint;
    }

    /**
     * Returns the owner of the signing key
     */
    public Name
    getSigner() {
        return signer;
    }

    /**
     * Returns the binary data representing the signature
     */
    public byte[]
    getSignature() {
        return signature;
    }

    void
    setSignature(byte[] signature) {
        this.signature = signature;
    }


}
