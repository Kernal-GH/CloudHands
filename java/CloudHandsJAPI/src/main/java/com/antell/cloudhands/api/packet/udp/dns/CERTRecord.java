package com.antell.cloudhands.api.packet.udp.dns;

import com.antell.cloudhands.api.utils.Base64;
import com.antell.cloudhands.api.utils.MessagePackUtil;
import com.antell.cloudhands.api.utils.Text;
import org.elasticsearch.common.xcontent.XContentBuilder;
import org.msgpack.core.MessageUnpacker;

import java.io.DataInput;
import java.io.IOException;

/**
 * Certificate Record  - Stores a certificate associated with a name.  The
 * certificate might also be associated with a KEYRecord.
 *
 * @see KEYRecord
 */

public class CERTRecord extends Record {

    public static class CertificateType {
        /**
         * Certificate type identifiers.  See RFC 4398 for more detail.
         */

        private CertificateType() {
        }

        /**
         * PKIX (X.509v3)
         */
        public static final int PKIX = 1;

        /**
         * Simple Public Key Infrastructure
         */
        public static final int SPKI = 2;

        /**
         * Pretty Good Privacy
         */
        public static final int PGP = 3;

        /**
         * URL of an X.509 data object
         */
        public static final int IPKIX = 4;

        /**
         * URL of an SPKI certificate
         */
        public static final int ISPKI = 5;

        /**
         * Fingerprint and URL of an OpenPGP packet
         */
        public static final int IPGP = 6;

        /**
         * Attribute Certificate
         */
        public static final int ACPKIX = 7;

        /**
         * URL of an Attribute Certificate
         */
        public static final int IACPKIX = 8;

        /**
         * Certificate format defined by URI
         */
        public static final int URI = 253;

        /**
         * Certificate format defined by OID
         */
        public static final int OID = 254;

        private static Mnemonic types = new Mnemonic("Certificate type",
                Mnemonic.CASE_UPPER);

        static {
            types.setMaximum(0xFFFF);
            types.setNumericAllowed(true);

            types.add(PKIX, "PKIX");
            types.add(SPKI, "SPKI");
            types.add(PGP, "PGP");
            types.add(PKIX, "IPKIX");
            types.add(SPKI, "ISPKI");
            types.add(PGP, "IPGP");
            types.add(PGP, "ACPKIX");
            types.add(PGP, "IACPKIX");
            types.add(URI, "URI");
            types.add(OID, "OID");
        }

        /**
         * Converts a certificate type into its textual representation
         */
        public static String string(int type) {
            return types.getText(type);
        }

        /**
         * Converts a textual representation of an certificate type into its
         * numeric code.  Integers in the range 0..65535 are also accepted.
         *
         * @param s The textual representation of the algorithm
         * @return The algorithm code, or -1 on error.
         */
        public static int value(String s) {
            return types.getValue(s);
        }
    }

    /**
     * PKIX (X.509v3)
     */
    public static final int PKIX = CertificateType.PKIX;

    /**
     * Simple Public Key Infrastructure
     */
    public static final int SPKI = CertificateType.SPKI;

    /**
     * Pretty Good Privacy
     */
    public static final int PGP = CertificateType.PGP;

    /**
     * Certificate format defined by URI
     */
    public static final int URI = CertificateType.URI;

    /**
     * Certificate format defined by IOD
     */
    public static final int OID = CertificateType.OID;

    private int certType, keyTag;
    private int alg;
    private byte[] cert;

    public CERTRecord() {
    }

    @Override
    public Record getObject() {
        return new CERTRecord();
    }

    @Override
    public void read(DataInput in) throws IOException {
        certType = in.readUnsignedShort();
        keyTag = in.readUnsignedShort();
        alg = in.readUnsignedByte();

        cert = Text.readBytes(in,2);
    }

    @Override
    public void read(MessageUnpacker unpacker) throws IOException {

        MessagePackUtil.parseMapHeader(unpacker,true);
        certType = MessagePackUtil.parseInt(unpacker);
        keyTag = MessagePackUtil.parseInt(unpacker);
        alg = MessagePackUtil.parseInt(unpacker);
        cert = MessagePackUtil.parseBin(unpacker);
    }

    /**
     * Converts rdata to a String
     */
    @Override
    public String rrToString() {
        StringBuffer sb = new StringBuffer();
        sb.append(certType);
        sb.append(" ");
        sb.append(keyTag);
        sb.append(" ");
        sb.append(alg);
        if (cert != null) {
            sb.append(" ");
            sb.append(Base64.toString(cert));
        }
        return sb.toString();
    }

    @Override
    XContentBuilder rdataToJson(XContentBuilder cb) throws IOException {

        cb.field("certType",certType);
        cb.field("keyTag",keyTag);
        cb.field("alg",alg);
        cb.field("cert",cert== null?"":Base64.toString(cert));

        return cb;
    }

    /**
     * Returns the type of certificate
     */
    public int
    getCertType() {
        return certType;
    }

    /**
     * Returns the ID of the associated KEYRecord, if present
     */
    public int
    getKeyTag() {
        return keyTag;
    }

    /**
     * Returns the algorithm of the associated KEYRecord, if present
     */
    public int
    getAlgorithm() {
        return alg;
    }

    /**
     * Returns the binary representation of the certificate
     */
    public byte[]
    getCert() {
        return cert;
    }

}
