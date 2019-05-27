package com.antell.cloudhands.api.sink.es;

/**
 * Created by shajf on 2016/5/20.
 */
public class SimpleIndexNameBuilder implements IndexNameBuilder{

    @Override
    public String build(String indexPrefix, String docType) {

        return indexPrefix;
    }

}
