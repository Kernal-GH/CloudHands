package com.antell.cloudhands.api.source;

import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.LinkedBlockingQueue;

/**
 * Created by dell on 2018/6/19.
 */
public abstract class AbstractSource implements Source {

    protected final LinkedBlockingQueue<SourceEntry> sourceEntries;
    private final List<SourceReader> sourceReaders;

    public AbstractSource() {

        this.sourceEntries = new LinkedBlockingQueue<>();
        this.sourceReaders = new ArrayList<>();
    }

    @Override
    public void addSourceReader(SourceReader sourceReader) {

        this.sourceReaders.add(sourceReader);
    }

    @Override
    public void start() throws SourceException {

        for(SourceReader sourceReader:sourceReaders){

            sourceReader.start();
        }
    }

    @Override
    public void stop() {

        for(SourceReader sourceReader:sourceReaders){

            sourceReader.stop();
        }
    }

    @Override
    public synchronized void put(SourceEntry entry) {

        try {
            sourceEntries.put(entry);
        } catch (InterruptedException e) {

        }
    }

    @Override
    public synchronized void put(List<SourceEntry> entries) {

        entries.forEach(e->put(e));

    }


    @Override
    public synchronized SourceEntry take() {

        if(sourceEntries.isEmpty())
            return null;

        try {
            return sourceEntries.take();
        } catch (InterruptedException e) {
            return null;
        }
    }

    @Override
    public void commit(SourceEntry entry) {

    }

}
