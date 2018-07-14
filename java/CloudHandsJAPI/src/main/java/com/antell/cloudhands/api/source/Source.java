package com.antell.cloudhands.api.source;

import com.antell.cloudhands.api.context.Context;

/**
 * Created by mala on 2018/6/18.
 */
public interface Source {

    /*init Source*/
    void init(Context context,Filter filter) throws SourceException;

    void start() throws SourceException;

    void stop();

    /*put a entry to source*/
     void put(SourceEntry entry);

    /*Get a entry from source*/
    SourceEntry take();

    /*Commit a session,*/
    void commit(SourceEntry entry);

    void addSourceReader(SourceReader sourceReader);
}









