package com.antell.cloudhands.api.geoip;

import org.elasticsearch.common.xcontent.XContentBuilder;

import java.io.IOException;

/**
 * Created by g on 7/6/17.
 */
public class GEOIPItem  {

    private String location;
    private String country;
    private String city;
    private double longitude;
    private double latitude;

    public GEOIPItem(){

        location = "CN";
        country = "中国";
        city = "北京";
        longitude = 0;
        latitude = 0;
    }

    public void appendToJson(XContentBuilder pcb,String name) throws IOException {

        XContentBuilder cb = pcb.startObject(name);
        cb.field("location",location);
        cb.field("country",country);
        cb.field("city",city);
        cb.field("longitude",longitude);
        cb.field("latitude",latitude);
        cb.endObject();
    }

    public void setLocation(String location) {
        this.location = location;
    }

    public String getLocation() {
        return location;
    }

    public String getCountry() {
        return country;
    }

    public void setCountry(String country) {
        this.country = country;
    }

    public String getCity() {
        return city;
    }

    public void setCity(String city) {
        this.city = city;
    }

    public double getLongitude() {
        return longitude;
    }

    public void setLongitude(double longitude) {
        this.longitude = longitude;
    }

    public double getLatitude() {
        return latitude;
    }

    public void setLatitude(double latitude) {
        this.latitude = latitude;
    }
}
