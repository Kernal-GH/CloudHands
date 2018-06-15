package com.antell.cloudhands.api;

import org.elasticsearch.common.xcontent.XContentBuilder;

import java.io.IOException;

/**
 * Created by dell on 2018/6/15.
 */
public interface DataOutJson {

    XContentBuilder dataToJson(XContentBuilder cb) throws IOException;

}
