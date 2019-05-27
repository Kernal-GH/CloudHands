package com.antell.cloudhands.api.source;

import com.antell.cloudhands.api.DataDump;
import com.antell.cloudhands.api.sink.es.ESIndexable;

import java.util.List;

/**
 * Created by dell on 2018/6/19.
 */
public interface SourceEntry extends DataDump, ESIndexable {

    default List<SourceEntry> generate(){
        return null;
    }

    String getObjectId();

    default String getProto(){

        return null;
    }

    default long getSrcIPI(){

        return 0;
    }

    default long getDstIPI(){
        return 0;
    }

    default long getTime(){

        return 0;
    }


}
