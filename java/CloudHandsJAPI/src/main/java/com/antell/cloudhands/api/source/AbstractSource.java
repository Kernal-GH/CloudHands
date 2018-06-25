package com.antell.cloudhands.api.source;

import java.util.concurrent.LinkedBlockingQueue;

/**
 * Created by dell on 2018/6/19.
 */
public abstract class AbstractSource implements Source {

    protected final LinkedBlockingQueue<SourceEntry> sourceEntries;
    private   SourceReader sourceReader;

    public AbstractSource() {

        this.sourceEntries = new LinkedBlockingQueue<>();
        this.sourceReader = null;
    }

    @Override
    public void setSourceReader(SourceReader sourceReader) {

        this.sourceReader = sourceReader;
    }

    @Override
    public void start() throws SourceException {

        sourceReader.start();
    }

    @Override
    public void stop() {

        sourceReader.stop();

    }

    @Override
    public synchronized void put(SourceEntry entry) {

        try {
            sourceEntries.put(entry);
        } catch (InterruptedException e) {

        }
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
