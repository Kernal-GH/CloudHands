package com.antell.cloudhands.api.sink.es;

/**
 * Created by shajf on 2016/5/20.
 */
public interface IndexNameBuilder {

    public String build(String indexPrefix, String docType);

}
