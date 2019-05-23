package com.antell.cloudhands.api.sink.es;


/**
 * Created by dell on 2017/4/21.
 */
public interface ESClient {


    void index(ESIndexable indexable);

    void close();
}
