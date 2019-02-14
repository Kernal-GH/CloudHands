package com.antell.cloudhands.api.packet.udp.dns;

import com.antell.cloudhands.api.utils.Base16;
import com.antell.cloudhands.api.utils.Text;
import org.elasticsearch.common.xcontent.XContentBuilder;

import java.io.DataInput;
import java.io.IOException;

/**
 * Transport Layer Security Authentication
 */

public class TLSARecord extends Record {

    public static class CertificateUsage {
        private CertificateUsage() {
        }

        public static final int CA_CONSTRAINT = 0;
        public static final int SERVICE_CERTIFICATE_CONSTRAINT = 1;
        public static final int TRUST_ANCHOR_ASSERTION = 2;
        public static final int DOMAIN_ISSUED_CERTIFICATE = 3;
    }

    public static class Selector {
        private Selector() {
        }

        /**
         * Full certificate; the Certificate binary structure defined in
         * [RFC5280]
         */
        public static final int FULL_CERTIFICATE = 0;

        /**
         * SubjectPublicKeyInfo; DER-encoded binary structure defined in
         * [RFC5280]
         */
        public static final int SUBJECT_PUBLIC_KEY_INFO = 1;
    }

    public static class MatchingType {
        private MatchingType() {
        }

        /**
         * Exact match on selected content
         */
        public static final int EXACT = 0;

        /**
         * SHA-256 hash of selected content [RFC6234]
         */
        public static final int SHA256 = 1;

        /**
         * SHA-512 hash of selected content [RFC6234]
         */
        public static final int SHA512 = 2;
    }

    private int certificateUsage;
    private int selector;
    private int matchingType;
    private byte[] certificateAssociationData;

    public TLSARecord() {
    }

    @Override
    public Record getObject() {
        return new TLSARecord();
    }

    @Override
    public void read(DataInput in) throws IOException {
        certificateUsage = in.readUnsignedByte();
        selector = in.readUnsignedByte();
        matchingType = in.readUnsignedByte();
        certificateAssociationData = Text.readBytes(in,2);
    }

    /**
     * Converts rdata to a String
     */
    @Override
    public String rrToString() {
        StringBuffer sb = new StringBuffer();
        sb.append(certificateUsage);
        sb.append(" ");
        sb.append(selector);
        sb.append(" ");
        sb.append(matchingType);
        sb.append(" ");
        sb.append(Base16.toString(certificateAssociationData));

        return sb.toString();
    }

    @Override
    public XContentBuilder rdataToJson(XContentBuilder cb) throws IOException {

        cb.field("certificateUsage",certificateUsage);
        cb.field("selector",selector);
        cb.field("matchingType",matchingType);
        cb.field("certificateAssociationData",Base16.toString(certificateAssociationData));
        return cb;
    }

    /**
     * Returns the certificate usage of the TLSA record
     */
    public int
    getCertificateUsage() {
        return certificateUsage;
    }

    /**
     * Returns the selector of the TLSA record
     */
    public int
    getSelector() {
        return selector;
    }

    /**
     * Returns the matching type of the TLSA record
     */
    public int
    getMatchingType() {
        return matchingType;
    }

    /**
     * Returns the certificate associate data of this TLSA record
     */
    public final byte[]
    getCertificateAssociationData() {
        return certificateAssociationData;
    }

}
