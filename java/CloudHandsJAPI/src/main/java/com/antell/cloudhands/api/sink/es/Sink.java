package com.antell.cloudhands.api.sink.es;


/**
 * Created by dell on 2017/4/13.
 */
public interface Sink {

    void start() throws SinkException;

    void stop();

}
