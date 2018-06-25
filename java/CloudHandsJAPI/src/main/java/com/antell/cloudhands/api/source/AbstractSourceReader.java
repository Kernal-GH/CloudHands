package com.antell.cloudhands.api.source;
import com.antell.cloudhands.api.packet.PacketRecord;

import java.util.concurrent.ScheduledThreadPoolExecutor;
import java.util.concurrent.TimeUnit;

/**
 * Created by dell on 2018/6/19.
 */
public abstract class AbstractSourceReader implements SourceReader{

    protected final Source source;
    protected final SourceEntryParser parser;
    protected final Filter filter;

    private ScheduledThreadPoolExecutor scheduledThreadPoolExecutor;
    private SourceReadRunnable sourceReadRunnable;

    protected AbstractSourceReader(Source source, SourceEntryParser parser, Filter filter) {
        this.source = source;
        this.parser = parser;
        this.filter = filter;
    }

    @Override
    public void start() throws SourceException{

        if(open() == -1){

            throw new SourceException("Cannot open source reader to read!");
        }

        scheduledThreadPoolExecutor = new ScheduledThreadPoolExecutor(1);

        sourceReadRunnable = new SourceReadRunnable();

        scheduledThreadPoolExecutor.scheduleAtFixedRate(sourceReadRunnable, 500, 500,
                TimeUnit.MILLISECONDS);

    }

    @Override
    public void stop() {

        close();
        scheduledThreadPoolExecutor.remove(sourceReadRunnable);
    }

    private class SourceReadRunnable implements Runnable{


        @Override
        public void run() {

            while (true) {
                PacketRecord packetRecord = read();
                if (packetRecord != null) {
                    if(filter!=null&&!filter.isAccept(packetRecord)){
                        readEnd(packetRecord);
                        continue;
                    }

                /*parse it */
                    try {

                        SourceEntry sourceEntry = parser.parse(packetRecord);
                        if(filter==null||filter.isAccept(sourceEntry))
                        {
                            source.put(sourceEntry);
                        }

                    } catch (Exception e) {
                        e.printStackTrace();
                        //log.error(e.getMessage());
                    /*ignore exceptions*/
                    } finally {
                        readEnd(packetRecord);
                    }
                }else{
                    /*na data to read,return thread fun and sleep some times*/
                    break;
                }
            }
        }

    };

}
