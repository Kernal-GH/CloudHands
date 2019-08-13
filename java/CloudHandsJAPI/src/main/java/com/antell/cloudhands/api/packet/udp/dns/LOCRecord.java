package com.antell.cloudhands.api.packet.udp.dns;

import com.antell.cloudhands.api.utils.MessagePackUtil;
import org.elasticsearch.common.xcontent.XContentBuilder;
import org.msgpack.core.MessageUnpacker;

import java.io.DataInput;
import java.io.IOException;
import java.text.DecimalFormat;
import java.text.NumberFormat;

/**
 * Location - describes the physical location of hosts, networks, subnets.
 */

public class LOCRecord extends Record {

    private static NumberFormat w2, w3;

    private long size, hPrecision, vPrecision;
    private long latitude, longitude, altitude;

    static {
        w2 = new DecimalFormat();
        w2.setMinimumIntegerDigits(2);

        w3 = new DecimalFormat();
        w3.setMinimumIntegerDigits(3);
    }

    public LOCRecord() {
    }

    @Override
    public Record getObject() {
        return new LOCRecord();
    }

    @Override
    public void read(DataInput in) throws IOException {
        int version;

        version = in.readUnsignedByte();
        if (version != 0)
            throw new ParseException("Invalid LOC version");

        size = parseLOCformat(in.readUnsignedByte());
        hPrecision = parseLOCformat(in.readUnsignedByte());
        vPrecision = parseLOCformat(in.readUnsignedByte());
        latitude = in.readInt();
        longitude = in.readInt();
        altitude = in.readInt();
    }

    @Override
    public void read(MessageUnpacker unpacker) throws IOException {

        MessagePackUtil.parseMapHeader(unpacker,true);
        int version = MessagePackUtil.parseInt(unpacker);
        if (version != 0)
            throw new ParseException("Invalid LOC version");

        size = MessagePackUtil.parseInt(unpacker);
        hPrecision = parseLOCformat(MessagePackUtil.parseInt(unpacker));
        vPrecision = parseLOCformat(MessagePackUtil.parseInt(unpacker));
        latitude = MessagePackUtil.parseLong(unpacker);
        longitude = MessagePackUtil.parseLong(unpacker);
        altitude = MessagePackUtil.parseLong(unpacker);

    }

    private double
    parseFixedPoint(String s) {
        if (s.matches("^-?\\d+$"))
            return Integer.parseInt(s);
        else if (s.matches("^-?\\d+\\.\\d*$")) {
            String[] parts = s.split("\\.");
            double value = Integer.parseInt(parts[0]);
            double fraction = Integer.parseInt(parts[1]);
            if (value < 0)
                fraction *= -1;
            int digits = parts[1].length();
            return value + (fraction / Math.pow(10, digits));
        } else
            throw new NumberFormatException();
    }

    private void
    renderFixedPoint(StringBuffer sb, NumberFormat formatter, long value,
                     long divisor) {
        sb.append(value / divisor);
        value %= divisor;
        if (value != 0) {
            sb.append(".");
            sb.append(formatter.format(value));
        }
    }

    private String
    positionToString(long value, char pos, char neg) {
        StringBuffer sb = new StringBuffer();
        char direction;

        long temp = value - (1L << 31);
        if (temp < 0) {
            temp = -temp;
            direction = neg;
        } else
            direction = pos;

        sb.append(temp / (3600 * 1000)); /* degrees */
        temp = temp % (3600 * 1000);
        sb.append(" ");

        sb.append(temp / (60 * 1000)); /* minutes */
        temp = temp % (60 * 1000);
        sb.append(" ");

        renderFixedPoint(sb, w3, temp, 1000); /* seconds */
        sb.append(" ");

        sb.append(direction);

        return sb.toString();
    }


    /**
     * Convert to a String
     */
    @Override
    public String rrToString() {
        StringBuffer sb = new StringBuffer();

	/* Latitude */
        sb.append(positionToString(latitude, 'N', 'S'));
        sb.append(" ");

	/* Latitude */
        sb.append(positionToString(longitude, 'E', 'W'));
        sb.append(" ");

	/* Altitude */
        renderFixedPoint(sb, w2, altitude - 10000000, 100);
        sb.append("m ");

	/* Size */
        renderFixedPoint(sb, w2, size, 100);
        sb.append("m ");

	/* Horizontal precision */
        renderFixedPoint(sb, w2, hPrecision, 100);
        sb.append("m ");

	/* Vertical precision */
        renderFixedPoint(sb, w2, vPrecision, 100);
        sb.append("m");

        return sb.toString();
    }

    @Override
    public XContentBuilder rdataToJson(XContentBuilder cb) throws IOException {

        StringBuffer sb = new StringBuffer();
        cb.field("latitude",positionToString(latitude, 'N', 'S'));
        cb.field("longitude",positionToString(longitude, 'E', 'W'));
        renderFixedPoint(sb, w2, altitude - 10000000, 100);
        cb.field("altitude",sb.toString());
        sb.setLength(0);
        renderFixedPoint(sb, w2, size, 100);
        cb.field("size",sb.toString());
        sb.setLength(0);
        renderFixedPoint(sb, w2, hPrecision, 100);
        cb.field("horizontal",sb.toString());
        sb.setLength(0);
        renderFixedPoint(sb, w2, vPrecision, 100);
        cb.field("vertical",sb.toString());

        return cb;
    }

    /**
     * Returns the latitude
     */
    public double
    getLatitude() {
        return ((double) (latitude - (1L << 31))) / (3600 * 1000);
    }

    /**
     * Returns the longitude
     */
    public double
    getLongitude() {
        return ((double) (longitude - (1L << 31))) / (3600 * 1000);
    }

    /**
     * Returns the altitude
     */
    public double
    getAltitude() {
        return ((double) (altitude - 10000000)) / 100;
    }

    /**
     * Returns the diameter of the enclosing sphere
     */
    public double
    getSize() {
        return ((double) size) / 100;
    }

    /**
     * Returns the horizontal precision
     */
    public double
    getHPrecision() {
        return ((double) hPrecision) / 100;
    }

    /**
     * Returns the horizontal precision
     */
    public double
    getVPrecision() {
        return ((double) vPrecision) / 100;
    }


    private static long
    parseLOCformat(int b) throws ParseException {
        long out = b >> 4;
        int exp = b & 0xF;
        if (out > 9 || exp > 9)
            throw new ParseException("Invalid LOC Encoding");
        while (exp-- > 0)
            out *= 10;
        return (out);
    }

    private int
    toLOCformat(long l) {
        byte exp = 0;
        while (l > 9) {
            exp++;
            l /= 10;
        }
        return (int) ((l << 4) + exp);
    }

}
