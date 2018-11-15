package com.antell.cloudhands.api.geoip;

import com.antell.cloudhands.api.context.Context;

/**
 * Created by dell on 2018/6/26.
 */
public interface GEOIPDataBase {

    void open(Context context) throws GEOIPException;

    GEOIPItem query(String ip);

    void close();
}
