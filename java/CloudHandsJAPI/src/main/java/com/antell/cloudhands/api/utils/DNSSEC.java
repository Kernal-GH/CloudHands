package com.antell.cloudhands.api.utils;

import com.antell.cloudhands.api.packet.udp.dns.*;
import com.google.common.io.ByteStreams;

import javax.xml.crypto.Data;
import java.io.DataInput;
import java.io.IOException;
import java.math.BigInteger;
import java.security.*;
import java.security.interfaces.*;
import java.security.spec.*;
import java.util.Arrays;
import java.util.Date;
import java.util.Iterator;

/**
 * Constants and methods relating to DNSSEC.
 * <p>
 * DNSSEC provides authentication for DNS information.
 *
 * @see RRSIGRecord
 * @see DNSKEYRecord
 * @see RRset
 */

public class DNSSEC {

    public static class Algorithm {
        private Algorithm() {
        }

        /**
         * RSA/MD5 public key (deprecated)
         */
        public static final int RSAMD5 = 1;

        /**
         * Diffie Hellman key
         */
        public static final int DH = 2;

        /**
         * DSA public key
         */
        public static final int DSA = 3;

        /**
         * RSA/SHA1 public key
         */
        public static final int RSASHA1 = 5;

        /**
         * DSA/SHA1, NSEC3-aware public key
         */
        public static final int DSA_NSEC3_SHA1 = 6;

        /**
         * RSA/SHA1, NSEC3-aware public key
         */
        public static final int RSA_NSEC3_SHA1 = 7;

        /**
         * RSA/SHA256 public key
         */
        public static final int RSASHA256 = 8;

        /**
         * RSA/SHA512 public key
         */
        public static final int RSASHA512 = 10;

        /**
         * GOST R 34.10-2001.
         * This requires an external cryptography provider,
         * such as BouncyCastle.
         */
        public static final int ECC_GOST = 12;

        /**
         * ECDSA Curve P-256 with SHA-256 public key
         **/
        public static final int ECDSAP256SHA256 = 13;

        /**
         * ECDSA Curve P-384 with SHA-384 public key
         **/
        public static final int ECDSAP384SHA384 = 14;

        /**
         * Indirect keys; the actual key is elsewhere.
         */
        public static final int INDIRECT = 252;

        /**
         * Private algorithm, specified by domain name
         */
        public static final int PRIVATEDNS = 253;

        /**
         * Private algorithm, specified by OID
         */
        public static final int PRIVATEOID = 254;

        private static Mnemonic algs = new Mnemonic("DNSSEC algorithm",
                Mnemonic.CASE_UPPER);

        static {
            algs.setMaximum(0xFF);
            algs.setNumericAllowed(true);

            algs.add(RSAMD5, "RSAMD5");
            algs.add(DH, "DH");
            algs.add(DSA, "DSA");
            algs.add(RSASHA1, "RSASHA1");
            algs.add(DSA_NSEC3_SHA1, "DSA-NSEC3-SHA1");
            algs.add(RSA_NSEC3_SHA1, "RSA-NSEC3-SHA1");
            algs.add(RSASHA256, "RSASHA256");
            algs.add(RSASHA512, "RSASHA512");
            algs.add(ECC_GOST, "ECC-GOST");
            algs.add(ECDSAP256SHA256, "ECDSAP256SHA256");
            algs.add(ECDSAP384SHA384, "ECDSAP384SHA384");
            algs.add(INDIRECT, "INDIRECT");
            algs.add(PRIVATEDNS, "PRIVATEDNS");
            algs.add(PRIVATEOID, "PRIVATEOID");
        }

        /**
         * Converts an algorithm into its textual representation
         */
        public static String
        string(int alg) {
            return algs.getText(alg);
        }

        /**
         * Converts a textual representation of an algorithm into its numeric
         * code.  Integers in the range 0..255 are also accepted.
         *
         * @param s The textual representation of the algorithm
         * @return The algorithm code, or -1 on error.
         */
        public static int
        value(String s) {
            return algs.getValue(s);
        }
    }

    private DNSSEC() {
    }

    /**
     * A DNSSEC exception.
     */
    public static class DNSSECException extends Exception {
        DNSSECException(String s) {
            super(s);
        }
    }

    /**
     * An algorithm is unsupported by this DNSSEC implementation.
     */
    public static class UnsupportedAlgorithmException extends DNSSECException {
        UnsupportedAlgorithmException(int alg) {
            super("Unsupported algorithm: " + alg);
        }
    }

    /**
     * The cryptographic data in a DNSSEC key is malformed.
     */
    public static class MalformedKeyException extends DNSSECException {
        MalformedKeyException(KEYBase rec) {
            super("Invalid key data: " + rec.rdataToString());
        }
    }


    /**
     * A DNSSEC verification failed because the signature has expired.
     */
    public static class SignatureExpiredException extends DNSSECException {
        private Date when, now;

        SignatureExpiredException(Date when, Date now) {
            super("signature expired");
            this.when = when;
            this.now = now;
        }

        /**
         * @return When the signature expired
         */
        public Date
        getExpiration() {
            return when;
        }

        /**
         * @return When the verification was attempted
         */
        public Date
        getVerifyTime() {
            return now;
        }
    }

    /**
     * A DNSSEC verification failed because the signature has not yet become valid.
     */
    public static class SignatureNotYetValidException extends DNSSECException {
        private Date when, now;

        SignatureNotYetValidException(Date when, Date now) {
            super("signature is not yet valid");
            this.when = when;
            this.now = now;
        }

        /**
         * @return When the signature will become valid
         */
        public Date
        getExpiration() {
            return when;
        }

        /**
         * @return When the verification was attempted
         */
        public Date
        getVerifyTime() {
            return now;
        }
    }

    /**
     * A DNSSEC verification failed because the cryptographic signature
     * verification failed.
     */
    public static class SignatureVerificationException extends DNSSECException {
        SignatureVerificationException() {
            super("signature verification failed");
        }
    }

    /**
     * The key data provided is inconsistent.
     */
    public static class IncompatibleKeyException extends IllegalArgumentException {
        IncompatibleKeyException() {
            super("incompatible keys");
        }
    }

    /**
     * No signature was found.
     */
    public static class NoSignatureException extends DNSSECException {
        NoSignatureException() {
            super("no signature found");
        }
    }

    private static int
    BigIntegerLength(BigInteger i) {
        return (i.bitLength() + 7) / 8;
    }

    private static BigInteger
    readBigInteger(DataInput in, int len) throws IOException {

        byte[] b = new byte[len];
        in.readFully(b,0,len);

        return new BigInteger(1, b);
    }

    private static BigInteger
    readBigInteger(DataInput in) {
        byte[] b = new byte[0];
        try {
            b = Text.readBytes(in,2);
        } catch (IOException e) {
            e.printStackTrace();
        }

        return new BigInteger(1, b);
    }

    private static byte[]
    trimByteArray(byte[] array) {
        if (array[0] == 0) {
            byte trimmedArray[] = new byte[array.length - 1];
            System.arraycopy(array, 1, trimmedArray, 0, array.length - 1);
            return trimmedArray;
        } else {
            return array;
        }
    }

    private static void
    reverseByteArray(byte[] array) {
        for (int i = 0; i < array.length / 2; i++) {
            int j = array.length - i - 1;
            byte tmp = array[i];
            array[i] = array[j];
            array[j] = tmp;
        }
    }

    private static BigInteger
    readBigIntegerLittleEndian(DataInput in, int len) throws IOException {
        byte[] b = new byte[len];
        in.readFully(b,0,len);

        reverseByteArray(b);
        return new BigInteger(1, b);
    }


    private static PublicKey
    toRSAPublicKey(KEYBase r) throws IOException, GeneralSecurityException {
        DataInput dataInput = ByteStreams.newDataInput(r.getKey());

        int exponentLength = dataInput.readUnsignedByte();
        if (exponentLength == 0)
            exponentLength = dataInput.readUnsignedShort();
        BigInteger exponent = readBigInteger(dataInput, exponentLength);
        BigInteger modulus = readBigInteger(dataInput);

        KeyFactory factory = KeyFactory.getInstance("RSA");
        return factory.generatePublic(new RSAPublicKeySpec(modulus, exponent));
    }

    private static PublicKey
    toDSAPublicKey(KEYBase r) throws IOException, GeneralSecurityException,
            MalformedKeyException {

        DataInput in = ByteStreams.newDataInput(r.getKey());

        int t = in.readUnsignedByte();
        if (t > 8)
            throw new MalformedKeyException(r);

        BigInteger q = readBigInteger(in, 20);
        BigInteger p = readBigInteger(in, 64 + t * 8);
        BigInteger g = readBigInteger(in, 64 + t * 8);
        BigInteger y = readBigInteger(in, 64 + t * 8);

        KeyFactory factory = KeyFactory.getInstance("DSA");
        return factory.generatePublic(new DSAPublicKeySpec(y, p, q, g));
    }

    private static class ECKeyInfo {
        int length;
        public BigInteger p, a, b, gx, gy, n;
        EllipticCurve curve;
        ECParameterSpec spec;

        ECKeyInfo(int length, String p_str, String a_str, String b_str,
                  String gx_str, String gy_str, String n_str) {
            this.length = length;
            p = new BigInteger(p_str, 16);
            a = new BigInteger(a_str, 16);
            b = new BigInteger(b_str, 16);
            gx = new BigInteger(gx_str, 16);
            gy = new BigInteger(gy_str, 16);
            n = new BigInteger(n_str, 16);
            curve = new EllipticCurve(new ECFieldFp(p), a, b);
            spec = new ECParameterSpec(curve, new ECPoint(gx, gy), n, 1);
        }
    }

    // RFC 4357 Section 11.4
    private static final ECKeyInfo GOST = new ECKeyInfo(32,
            "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFD97",
            "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFD94",
            "A6",
            "1",
            "8D91E471E0989CDA27DF505A453F2B7635294F2DDF23E3B122ACC99C9E9F1E14",
            "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF6C611070995AD10045841B09B761B893");

    // RFC 5114 Section 2.6
    private static final ECKeyInfo ECDSA_P256 = new ECKeyInfo(32,
            "FFFFFFFF00000001000000000000000000000000FFFFFFFFFFFFFFFFFFFFFFFF",
            "FFFFFFFF00000001000000000000000000000000FFFFFFFFFFFFFFFFFFFFFFFC",
            "5AC635D8AA3A93E7B3EBBD55769886BC651D06B0CC53B0F63BCE3C3E27D2604B",
            "6B17D1F2E12C4247F8BCE6E563A440F277037D812DEB33A0F4A13945D898C296",
            "4FE342E2FE1A7F9B8EE7EB4A7C0F9E162BCE33576B315ECECBB6406837BF51F5",
            "FFFFFFFF00000000FFFFFFFFFFFFFFFFBCE6FAADA7179E84F3B9CAC2FC632551");

    // RFC 5114 Section 2.7
    private static final ECKeyInfo ECDSA_P384 = new ECKeyInfo(48,
            "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFFFF0000000000000000FFFFFFFF",
            "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFFFF0000000000000000FFFFFFFC",
            "B3312FA7E23EE7E4988E056BE3F82D19181D9C6EFE8141120314088F5013875AC656398D8A2ED19D2A85C8EDD3EC2AEF",
            "AA87CA22BE8B05378EB1C71EF320AD746E1D3B628BA79B9859F741E082542A385502F25DBF55296C3A545E3872760AB7",
            "3617DE4A96262C6F5D9E98BF9292DC29F8F41DBD289A147CE9DA3113B5F0B8C00A60B1CE1D7E819D7A431D7C90EA0E5F",
            "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFC7634D81F4372DDF581A0DB248B0A77AECEC196ACCC52973");

    private static PublicKey
    toECGOSTPublicKey(KEYBase r, ECKeyInfo keyinfo) throws IOException,
            GeneralSecurityException, MalformedKeyException {
        DataInput in = ByteStreams.newDataInput(r.getKey());

        BigInteger x = readBigIntegerLittleEndian(in, keyinfo.length);
        BigInteger y = readBigIntegerLittleEndian(in, keyinfo.length);
        ECPoint q = new ECPoint(x, y);

        KeyFactory factory = KeyFactory.getInstance("ECGOST3410");
        return factory.generatePublic(new ECPublicKeySpec(q, keyinfo.spec));
    }

    private static PublicKey
    toECDSAPublicKey(KEYBase r, ECKeyInfo keyinfo) throws IOException,
            GeneralSecurityException, MalformedKeyException {

        DataInput in = ByteStreams.newDataInput(r.getKey());

        // RFC 6605 Section 4
        BigInteger x = readBigInteger(in, keyinfo.length);
        BigInteger y = readBigInteger(in, keyinfo.length);
        ECPoint q = new ECPoint(x, y);

        KeyFactory factory = KeyFactory.getInstance("EC");
        return factory.generatePublic(new ECPublicKeySpec(q, keyinfo.spec));
    }

    /**
     * Converts a KEY/DNSKEY record into a PublicKey
     */
    public static PublicKey
    toPublicKey(KEYBase r) throws DNSSECException {
        int alg = r.getAlgorithm();
        try {
            switch (alg) {
                case Algorithm.RSAMD5:
                case Algorithm.RSASHA1:
                case Algorithm.RSA_NSEC3_SHA1:
                case Algorithm.RSASHA256:
                case Algorithm.RSASHA512:
                    return toRSAPublicKey(r);
                case Algorithm.DSA:
                case Algorithm.DSA_NSEC3_SHA1:
                    return toDSAPublicKey(r);
                case Algorithm.ECC_GOST:
                    return toECGOSTPublicKey(r, GOST);
                case Algorithm.ECDSAP256SHA256:
                    return toECDSAPublicKey(r, ECDSA_P256);
                case Algorithm.ECDSAP384SHA384:
                    return toECDSAPublicKey(r, ECDSA_P384);
                default:
                    throw new UnsupportedAlgorithmException(alg);
            }
        } catch (IOException e) {
            throw new MalformedKeyException(r);
        } catch (GeneralSecurityException e) {
            throw new DNSSECException(e.toString());
        }
    }


    /**
     * Convert an algorithm number to the corresponding JCA string.
     *
     * @param alg The algorithm number.
     * @throws UnsupportedAlgorithmException The algorithm is unknown.
     */
    public static String
    algString(int alg) throws UnsupportedAlgorithmException {
        switch (alg) {
            case Algorithm.RSAMD5:
                return "MD5withRSA";
            case Algorithm.DSA:
            case Algorithm.DSA_NSEC3_SHA1:
                return "SHA1withDSA";
            case Algorithm.RSASHA1:
            case Algorithm.RSA_NSEC3_SHA1:
                return "SHA1withRSA";
            case Algorithm.RSASHA256:
                return "SHA256withRSA";
            case Algorithm.RSASHA512:
                return "SHA512withRSA";
            case Algorithm.ECC_GOST:
                return "GOST3411withECGOST3410";
            case Algorithm.ECDSAP256SHA256:
                return "SHA256withECDSA";
            case Algorithm.ECDSAP384SHA384:
                return "SHA384withECDSA";
            default:
                throw new UnsupportedAlgorithmException(alg);
        }
    }

    private static final int ASN1_SEQ = 0x30;
    private static final int ASN1_INT = 0x2;

    private static final int DSA_LEN = 20;

    private static byte[]
    ECGOSTSignaturefromDNS(byte[] signature, ECKeyInfo keyinfo)
            throws DNSSECException, IOException {
        if (signature.length != keyinfo.length * 2)
            throw new SignatureVerificationException();
        // Wire format is equal to the engine input
        return signature;
    }

    static void
    checkAlgorithm(PrivateKey key, int alg) throws UnsupportedAlgorithmException {
        switch (alg) {
            case Algorithm.RSAMD5:
            case Algorithm.RSASHA1:
            case Algorithm.RSA_NSEC3_SHA1:
            case Algorithm.RSASHA256:
            case Algorithm.RSASHA512:
                if (!(key instanceof RSAPrivateKey))
                    throw new IncompatibleKeyException();
                break;
            case Algorithm.DSA:
            case Algorithm.DSA_NSEC3_SHA1:
                if (!(key instanceof DSAPrivateKey))
                    throw new IncompatibleKeyException();
                break;
            case Algorithm.ECC_GOST:
            case Algorithm.ECDSAP256SHA256:
            case Algorithm.ECDSAP384SHA384:
                if (!(key instanceof ECPrivateKey))
                    throw new IncompatibleKeyException();
                break;
            default:
                throw new UnsupportedAlgorithmException(alg);
        }
    }


}
