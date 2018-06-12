package com.antell.cloudhands.api.packet.udp.dns;

import com.antell.security.utils.Base64;
import org.elasticsearch.common.xcontent.XContentBuilder;

import java.io.IOException;

/**
 * OPENPGPKEY Record - Stores an OpenPGP certificate associated with a name.
 * RFC 7929.
 */
public class OPENPGPKEYRecord extends Record {

    private static final long serialVersionUID = -1277262990243423062L;

    private byte[] cert;

    public OPENPGPKEYRecord() {
    }

    @Override
    public Record getObject() {
        return new OPENPGPKEYRecord();
    }

    /**
     * Creates an OPENPGPKEY Record from the given data
     *
     * @param cert Binary data representing the certificate
     */
    public OPENPGPKEYRecord(Name name, int dclass, long ttl, byte[] cert) {
        super(name, Type.OPENPGPKEY, dclass, ttl);
        this.cert = cert;
    }

    @Override
    public void rrFromWire(DNSInput in) throws IOException {
        cert = in.readByteArray();
    }

    @Override
    public void rdataFromString(Tokenizer st, Name origin) throws IOException {
        cert = st.getBase64();
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

    @Override
    public void rrToWire(DNSOutput out, Compression c, boolean canonical) {
        out.writeByteArray(cert);
    }

}
