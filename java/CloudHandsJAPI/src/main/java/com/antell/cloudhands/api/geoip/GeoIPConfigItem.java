package com.antell.cloudhands.api.geoip;

public class GeoIPConfigItem {

    private String dbType;
    private String dbPath;

    public GeoIPConfigItem(){

        dbType = "maxmind";
        dbPath = "";
    }
    public String getDbType() {
        return dbType;
    }

    public void setDbType(String dbType) {
        this.dbType = dbType;
    }

    public String getDbPath() {
        return dbPath;
    }

    public void setDbPath(String dbPath) {
        this.dbPath = dbPath;
    }
}
