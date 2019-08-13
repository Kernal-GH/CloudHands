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
 * Transaction Key - used to compute and/or securely transport a shared
 * secret to be used with TSIG.
 *
 */

public class TKEYRecord extends Record {

    private Name alg;
    private Date timeInception;
    private Date timeExpire;
    private int mode, error;
    private byte[] key;
    private byte[] other;

    /**
     * The key is assigned by the server (unimplemented)
     */
    public static final int SERVERASSIGNED = 1;

    /**
     * The key is computed using a Diffie-Hellman key exchange
     */
    public static final int DIFFIEHELLMAN = 2;

    /**
     * The key is computed using GSS_API (unimplemented)
     */
    public static final int GSSAPI = 3;

    /**
     * The key is assigned by the resolver (unimplemented)
     */
    public static final int RESOLVERASSIGNED = 4;

    /**
     * The key should be deleted
     */
    public static final int DELETE = 5;

    public TKEYRecord() {
    }

    @Override
    public Record getObject() {
        return new TKEYRecord();
    }

    @Override
    public void read(DataInput in) throws IOException {
        alg = new Name(in);
        timeInception = new Date(1000 * in.readInt());
        timeExpire = new Date(1000 * in.readInt());
        mode = in.readUnsignedShort();
        error = in.readUnsignedShort();

        key = Text.readBytes(in,2);
        other = Text.readBytes(in,2);

    }

    @Override
    public void read(MessageUnpacker unpacker) throws IOException {

        MessagePackUtil.parseMapHeader(unpacker,true);
        alg = new Name(unpacker);
        timeInception = new Date(1000 * MessagePackUtil.parseLong(unpacker));
        timeExpire = new Date(1000 * MessagePackUtil.parseLong(unpacker));
        mode = MessagePackUtil.parseInt(unpacker);
        error = MessagePackUtil.parseInt(unpacker);

        key = MessagePackUtil.parseBin(unpacker);
        other = MessagePackUtil.parseBin(unpacker);
    }

    protected String modeString() {
        switch (mode) {
            case SERVERASSIGNED:
                return "SERVERASSIGNED";
            case DIFFIEHELLMAN:
                return "DIFFIEHELLMAN";
            case GSSAPI:
                return "GSSAPI";
            case RESOLVERASSIGNED:
                return "RESOLVERASSIGNED";
            case DELETE:
                return "DELETE";
            default:
                return Integer.toString(mode);
        }
    }

    /**
     * Converts rdata to a String
     */
    @Override
    public String rrToString() {
        StringBuffer sb = new StringBuffer();
        sb.append(alg);
        sb.append(" ");
        sb.append(FormattedTime.format(timeInception));
        sb.append(" ");
        sb.append(FormattedTime.format(timeExpire));
        sb.append(" ");
        sb.append(modeString());
        sb.append(" ");
        sb.append(Rcode.TSIGstring(error));
        sb.append(" ");
        if (key != null) {
            sb.append(Base64.toString(key));
                sb.append(" ");
        }
        if (other != null)
            sb.append(Base64.toString(other));
        return sb.toString();
    }

    @Override
    public XContentBuilder rdataToJson(XContentBuilder cb) throws IOException {

        cb.field("alg",alg);
        cb.field("timeInception",FormattedTime.format(timeInception));
        cb.field("timeExpire",FormattedTime.format(timeExpire));
        cb.field("mode",modeString());
        cb.field("error",Rcode.TSIGstring(error));
        cb.field("key",key == null?"":Base64.toString(key));
        cb.field("other",other==null?"":Base64.toString(other));

        return cb;
    }

    /**
     * Returns the shared key's algorithm
     */
    public Name
    getAlgorithm() {
        return alg;
    }

    /**
     * Returns the beginning of the validity period of the shared secret or
     * keying material
     */
    public Date
    getTimeInception() {
        return timeInception;
    }

    /**
     * Returns the end of the validity period of the shared secret or
     * keying material
     */
    public Date
    getTimeExpire() {
        return timeExpire;
    }

    /**
     * Returns the key agreement mode
     */
    public int
    getMode() {
        return mode;
    }

    /**
     * Returns the extended error
     */
    public int
    getError() {
        return error;
    }

    /**
     * Returns the shared secret or keying material
     */
    public byte[]
    getKey() {
        return key;
    }

    /**
     * Returns the other data
     */
    public byte[]
    getOther() {
        return other;
    }


}
