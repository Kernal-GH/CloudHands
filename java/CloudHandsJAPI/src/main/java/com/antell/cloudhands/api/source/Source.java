package com.antell.cloudhands.api.source;

import com.antell.cloudhands.api.context.Context;

import java.util.List;

/**
 * Created by mala on 2018/6/18.
 */
public interface Source {

    /*init Source*/
    void init(Context context,Filter filter) throws SourceException;
    void init(List<String> mmapFiles,Filter filter) throws SourceException;

    void start() throws SourceException;

    void stop();

    /*put a entry to source*/
     void put(SourceEntry entry);

     void put(List<SourceEntry> entries);

    /*Get a entry from source*/
    SourceEntry take();

    /*Commit a session,*/
    void commit(SourceEntry entry);

    void addSourceReader(SourceReader sourceReader);
}









