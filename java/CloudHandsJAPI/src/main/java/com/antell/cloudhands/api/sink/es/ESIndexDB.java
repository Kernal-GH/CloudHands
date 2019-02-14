package com.antell.cloudhands.api.sink.es;

/**
 * Created by dell on 2018/6/25.
 */
public interface ESIndexDB {

    String getIndexPrefix();
    String getDocType();
    String getMapping();


}
