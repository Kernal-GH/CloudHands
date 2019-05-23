package com.antell.cloudhands.api.geoip;


/**
 * Created by dell on 2018/6/26.
 */
public interface GEOIPDataBase {

    void open(String dbPath) throws GEOIPException;

    GEOIPItem query(String ip);

    void close();
}
