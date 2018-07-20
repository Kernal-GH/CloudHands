package com.antell.cloudhands.api.source;

import com.antell.cloudhands.api.DataDump;
import com.antell.cloudhands.api.ESIndexable;

import java.util.List;

/**
 * Created by dell on 2018/6/19.
 */
public interface SourceEntry extends DataDump,ESIndexable {

    default List<SourceEntry> generate(){
        return null;
    }
}
