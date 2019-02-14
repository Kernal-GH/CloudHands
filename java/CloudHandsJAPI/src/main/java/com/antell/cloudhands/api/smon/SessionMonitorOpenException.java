package com.antell.cloudhands.api.smon;

import java.io.IOException;

/**
 * Created by dell on 2018/7/13.
 */
public class SessionMonitorOpenException extends IOException{

    public SessionMonitorOpenException(String msg){

        super(msg);
    }

}
