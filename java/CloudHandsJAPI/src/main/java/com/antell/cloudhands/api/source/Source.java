package com.antell.cloudhands.api.source;

import com.antell.cloudhands.api.context.Context;

/**
 * Created by mala on 2018/6/18.
 */
public interface Source {

    boolean isAccept();

    /*init Source*/
    void init(Context context) throws SourceException;

    void start() throws SourceException;

    void stop();

    /*Get a session from source*/
    Session take();

    /*Commit a session,*/
    void commit(Session session);
}









