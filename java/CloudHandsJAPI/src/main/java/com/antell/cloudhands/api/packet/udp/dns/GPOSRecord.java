package com.antell.cloudhands.api.packet.udp.dns;

import com.antell.cloudhands.api.utils.MessagePackUtil;
import com.antell.cloudhands.api.utils.Text;
import org.elasticsearch.common.xcontent.XContentBuilder;
import org.msgpack.core.MessageUnpacker;

import java.io.DataInput;
import java.io.IOException;

/**
 * Geographical Location - describes the physical location of a host.
 */

public class GPOSRecord extends Record {

    private String latitude;
    private String longitude;
    private String altitude;

    public GPOSRecord() {

    }

    @Override
    public Record getObject() {
        return new GPOSRecord();
    }

    private void validate(double longitude, double latitude) throws IllegalArgumentException {
        if (longitude < -90.0 || longitude > 90.0) {
            throw new IllegalArgumentException("illegal longitude " +
                    longitude);
        }
        if (latitude < -180.0 || latitude > 180.0) {
            throw new IllegalArgumentException("illegal latitude " +
                    latitude);
        }
    }

    @Override
    public void read(DataInput in) throws IOException {
        longitude = Text.readString(in,2);
        latitude = Text.readString(in,2);
        altitude = Text.readString(in,2);

        try {
            validate(getLongitude(), getLatitude());
        } catch (IllegalArgumentException e) {
            throw new ParseException(e.getMessage());
        }
    }

    @Override
    public void read(MessageUnpacker unpacker) throws IOException {

        MessagePackUtil.parseMapHeader(unpacker,true);
        longitude = MessagePackUtil.parseText(unpacker);
        latitude = MessagePackUtil.parseText(unpacker);
        altitude = MessagePackUtil.parseText(unpacker);
    }

    /**
     * Convert to a String
     */
    @Override
    public String rrToString() {
        StringBuffer sb = new StringBuffer();
        sb.append(longitude);
        sb.append(" ");
        sb.append(latitude);
        sb.append(" ");
        sb.append(altitude);
        return sb.toString();
    }

    @Override
    XContentBuilder rdataToJson(XContentBuilder cb) throws IOException {

        cb.field("longitude",longitude);
        cb.field("latitude",latitude);
        cb.field("altitude",altitude);
        return cb;

    }

    /**
     * Returns the longitude as a string
     */
    public String getLongitudeString() {

        return longitude;
    }

    /**
     * Returns the longitude as a double
     *
     * @throws NumberFormatException The string does not contain a valid numeric
     *                               value.
     */
    public double
    getLongitude() {
        return Double.parseDouble(getLongitudeString());
    }

    /**
     * Returns the latitude as a string
     */
    public String
    getLatitudeString() {
        return latitude;
    }

    /**
     * Returns the latitude as a double
     *
     * @throws NumberFormatException The string does not contain a valid numeric
     *                               value.
     */
    public double
    getLatitude() {
        return Double.parseDouble(getLatitudeString());
    }

    /**
     * Returns the altitude as a string
     */
    public String
    getAltitudeString() {
        return altitude;
    }

    /**
     * Returns the altitude as a double
     *
     * @throws NumberFormatException The string does not contain a valid numeric
     *                               value.
     */
    public double
    getAltitude() {
        return Double.parseDouble(getAltitudeString());
    }


}
