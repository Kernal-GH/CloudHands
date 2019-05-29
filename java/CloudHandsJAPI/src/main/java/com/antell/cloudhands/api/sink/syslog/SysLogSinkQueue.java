package com.antell.cloudhands.api.sink.syslog;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.util.concurrent.LinkedBlockingQueue;

public class SysLogSinkQueue {
    private final static Logger log = LoggerFactory.getLogger(SysLogSinkQueue.class);

    private LinkedBlockingQueue<String> queue;

    public SysLogSinkQueue(SysLogConfigItem sysLogConfigItem) {

        int qnum = sysLogConfigItem.getQueueLimits();

        if (qnum > 0)
            queue = new LinkedBlockingQueue<String>(qnum);
        else
            queue = new LinkedBlockingQueue<String>();
    }

    public void put(Object message) {
        try {
            queue.put((String) message);
        } catch (InterruptedException e) {
            log.error(e.getMessage());
            e.printStackTrace();
        }
    }

    public String take() {
        if (queue.isEmpty()) {

            return null;
        }

        String ans = null;

        try {
            ans = queue.take();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }

        return ans;
    }

    public boolean isEmpty() {
        return queue.isEmpty();
    }

    public long getCount() {
        return queue.size();
    }

}
