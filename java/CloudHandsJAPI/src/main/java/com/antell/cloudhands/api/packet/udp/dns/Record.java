package com.antell.cloudhands.api.packet.udp.dns;

import com.antell.cloudhands.api.utils.Base16;
import com.antell.cloudhands.api.utils.MessagePackUtil;
import com.google.common.base.Preconditions;
import org.elasticsearch.common.xcontent.XContentBuilder;
import org.msgpack.core.MessageUnpacker;

import java.io.DataInput;
import java.io.IOException;
import java.text.DecimalFormat;

/**
 * A generic DNS resource record.  The specific record types extend this class.
 * A record contains a name, type, class, ttl, and rdata.
 *
 */

public abstract class Record {

    protected Name name;
    protected int type;
    protected int dclass;
    protected long ttl;

    private static final DecimalFormat byteFormat = new DecimalFormat();

    static {
        byteFormat.setMinimumIntegerDigits(3);
    }

    protected Record() {

    }

    public Record(Name name, int type, int dclass, long ttl) {

        Type.check(type);
        DClass.check(dclass);
        TTL.check(ttl);
        this.name = name;
        this.type = type;
        this.dclass = dclass;
        this.ttl = ttl;
    }

    public abstract Record getObject();

    private static final Record getEmptyRecord(Name name, int type, int dclass, long ttl, boolean hasData) {
        Record proto, rec;

        if (hasData) {
            proto = Type.getProto(type);
            if (proto != null)
                rec = proto.getObject();
            else
                rec = new UNKRecord();
        } else
            rec = new EmptyRecord();
        rec.name = name;
        rec.type = type;
        rec.dclass = dclass;
        rec.ttl = ttl;
        return rec;
    }

    /**
     * Converts the type-specific RR to wire format - must be overriden
     */
    public abstract void read(DataInput in) throws IOException;
    public abstract void read(MessageUnpacker unpacker) throws IOException;

    private static Record newRecord(Name name, int type, int dclass, long ttl, DataInput in)
            throws IOException {


        Record rec;
        rec = getEmptyRecord(name, type, dclass, ttl, in != null);

        if (in != null) {

            rec.read(in);
        }

        return rec;
    }

    private static Record newRecord(Name name, int type, int dclass, long ttl, MessageUnpacker unpacker)
            throws IOException {

        Record rec;
        rec = getEmptyRecord(name, type, dclass, ttl, true);

        rec.read(unpacker);

        return rec;
    }



    /**
     * Creates a new empty record, with the given parameters.
     *
     * @param name   The owner name of the record.
     * @param type   The record's type.
     * @param dclass The record's class.
     * @param ttl    The record's time to live.
     * @return An object of a subclass of Record
     */
    public static Record newRecord(Name name, int type, int dclass, long ttl) {
        Type.check(type);
        DClass.check(dclass);
        TTL.check(ttl);

        return getEmptyRecord(name, type, dclass, ttl, false);
    }

    /**
     * Creates a new empty record, with the given parameters.  This method is
     * designed to create records that will be added to the QUERY section
     * of a message.
     *
     * @param name   The owner name of the record.
     * @param type   The record's type.
     * @param dclass The record's class.
     * @return An object of a subclass of Record
     */
    public static Record newRecord(Name name, int type, int dclass) {
        return newRecord(name, type, dclass, 0);
    }

    public static Record build(DataInput in) throws IOException {

        int type, dclass;
        long ttl;
        int length;
        Name name;
        Record rec;
        boolean hasData;

        name = new Name(in);
        type = in.readUnsignedShort();
        dclass = in.readUnsignedShort();
        ttl = in.readInt();
        hasData = (in.readUnsignedByte()==1);

        if (!hasData)
            return newRecord(name, type, dclass, ttl);

        rec = newRecord(name, type, dclass, ttl,in);

        return rec;
    }

    public static Record build(MessageUnpacker unpacker) throws IOException {

        int type, dclass;
        long ttl;
        int length;
        Name name;
        Record rec;
        boolean hasData;

        int n = MessagePackUtil.parseMapHeader(unpacker,true);
        Preconditions.checkArgument(n==6,"Invalid msgpack packet of dns record entry:"+n);
        name = new Name(unpacker);
        type = MessagePackUtil.parseInt(unpacker);
        dclass = MessagePackUtil.parseInt(unpacker);
        ttl = MessagePackUtil.parseLong(unpacker);
        hasData = MessagePackUtil.parseByte(unpacker)==1;
        if (!hasData)
            return newRecord(name, type, dclass, ttl);
        rec = newRecord(name, type, dclass, ttl,unpacker);

        return rec;
    }

    /**
     * Converts the type-specific RR to text format - must be overriden
     */
    abstract String rrToString();

    abstract XContentBuilder rdataToJson(XContentBuilder cb) throws IOException;

    /**
     * Converts a Record into a Json format
     * */
    public XContentBuilder toJson(XContentBuilder cb) throws IOException {

        cb.field("name",name.toString());
        cb.field("ttl",ttl);
        cb.field("dclassStr",DClass.string(dclass));
        cb.field("dclass",dclass);
        cb.field("typeStr",Type.string(type));
        cb.field("type",type);
        XContentBuilder cbb = cb.startObject("rdata");
        rdataToJson(cbb);
        cbb.endObject();

        return cb;
    }

    /**
     * Converts the rdata portion of a Record into a String representation
     */
    public String rdataToString() {
        return rrToString();
    }

    /**
     * Converts a Record into a String representation
     */
    public String toString() {
        StringBuffer sb = new StringBuffer();
        sb.append(name);
        if (sb.length() < 8)
            sb.append("\t");
        if (sb.length() < 16)
            sb.append("\t");
        sb.append("\t");
        sb.append(ttl);
        sb.append("\t");
        sb.append(DClass.string(dclass));
        sb.append("\t");

        sb.append(Type.string(type));
        String rdata = rrToString();
        if (!rdata.equals("")) {
            sb.append("\t");
            sb.append(rdata);
        }
        return sb.toString();
    }


    /**
     * Returns the record's name
     *
     * @see Name
     */
    public Name getName() {
        return name;
    }

    /**
     * Returns the record's type
     *
     * @see Type
     */
    public int getType() {
        return type;
    }

    /**
     * Returns the type of RRset that this record would belong to.  For all types
     * except RRSIG, this is equivalent to getType().
     *
     * @return The type of record, if not RRSIG.  If the type is RRSIG,
     * the type covered is returned.
     * @see Type
     * @see RRset
     */
    public int getRRsetType() {
        return type;
    }

    /**
     * Returns the record's class
     */
    public int getDClass() {
        return dclass;
    }

    /**
     * Returns the record's TTL
     */
    public long getTTL() {
        return ttl;
    }
    /**
     * Returns the name for which additional data processing should be done
     * for this record.  This can be used both for building responses and
     * parsing responses.
     *
     * @return The name to used for additional data processing, or null if this
     * record type does not require additional data processing.
     */
    public Name getAdditionalName() {
        return null;
    }
    /**
     * Determines if two Records could be part of the same RRset.
     * This compares the name, type, and class of the Records; the ttl and
     * rdata are not compared.
     */
    public boolean sameRRset(Record rec) {
        return (getRRsetType() == rec.getRRsetType() &&
                dclass == rec.dclass &&
                name.equals(rec.name));
    }

    /* Sets the TTL to the specified value.  This is intentionally not public. */
    public void setTTL(long ttl) {
        this.ttl = ttl;
    }

    /* Checks that an int contains an unsigned 8 bit value */
    public static int checkU8(String field, int val) {
        if (val < 0 || val > 0xFF)
            throw new IllegalArgumentException("\"" + field + "\" " + val +
                    " must be an unsigned 8 " +
                    "bit value");
        return val;
    }

    /* Checks that an int contains an unsigned 16 bit value */
    public static int checkU16(String field, int val) {
        if (val < 0 || val > 0xFFFF)
            throw new IllegalArgumentException("\"" + field + "\" " + val +
                    " must be an unsigned 16 " +
                    "bit value");
        return val;
    }

    /* Checks that a long contains an unsigned 32 bit value */
    public static long checkU32(String field, long val) {
        if (val < 0 || val > 0xFFFFFFFFL)
            throw new IllegalArgumentException("\"" + field + "\" " + val +
                    " must be an unsigned 32 " +
                    "bit value");
        return val;
    }

    /* Checks that a name is absolute */
    public static Name checkName(String field, Name name) {
        if (!name.isAbsolute())
            throw new RelativeNameException(name);
        return name;
    }

    public static byte[] checkByteArrayLength(String field, byte[] array, int maxLength) {
        if (array.length > 0xFFFF)
            throw new IllegalArgumentException("\"" + field + "\" array " +
                    "must have no more than " +
                    maxLength + " elements");
        byte[] out = new byte[array.length];
        System.arraycopy(array, 0, out, 0, array.length);
        return out;
    }

    /**
     * Converts a byte array into the unknown RR format.
     */
    protected static String unknownToString(byte[] data) {
        StringBuffer sb = new StringBuffer();
        sb.append("\\# ");
        sb.append(data.length);
        sb.append(" ");
        sb.append(Base16.toString(data));

        return sb.toString();
    }

   protected Record
    cloneRecord() {
        try {
            return (Record) clone();
        }
        catch (CloneNotSupportedException e) {
            throw new IllegalStateException();
        }
    }
}
