package com.antell.cloudhands.api.geoip;

import java.io.IOException;

/**
 * Created by dell on 2018/6/26.
 */
public class GEOIPException extends IOException {

    public GEOIPException(String errMessage){

        super(errMessage);
    }


}
