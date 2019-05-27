package com.antell.cloudhands.api.sink.es;

import java.util.List;

/**
 * Created by dell on 2018/6/27.
 */
public interface SinkQueue {



    void put(ESIndexable entry);

    void put(List<ESIndexable> arr);

    ESIndexable take();

    boolean isEmpty();

    long getCount();


}
