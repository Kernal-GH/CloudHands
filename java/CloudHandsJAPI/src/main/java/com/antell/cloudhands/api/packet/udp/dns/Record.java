package com.antell.cloudhands.api.packet.udp.dns;

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

    private static Record newRecord(Name name, int type, int dclass, long ttl, DataInput in)
            throws IOException {


        Record rec;
        rec = getEmptyRecord(name, type, dclass, ttl, in != null);

        if (in != null) {

            rec.read(in);
        }

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


    /**
     * Converts the type-specific RR to text format - must be overriden
     */
    abstract String rrToString();

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


}
