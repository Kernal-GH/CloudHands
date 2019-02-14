package com.antell.cloudhands.api.sink.es;

import com.antell.cloudhands.api.ESIndexable;
import org.elasticsearch.common.xcontent.XContentBuilder;

import java.io.IOException;

/**
 * Created by dell on 2018/6/21.
 */
public interface ESIndexEntry {

    void append(ESIndexable indexItem,String name) throws IOException;

    XContentBuilder toJson() throws IOException;

    ESIndexDB getIndexDB();

    String getId();

}
