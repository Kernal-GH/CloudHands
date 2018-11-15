package com.antell.CloudHandsJAPI.kafka;

import java.util.concurrent.CountDownLatch;

/**
 * Created by shajf on 2016/5/18.
 */
public class KeepAliveThread {

    private final CountDownLatch keepAliveLatch;
    private final Thread thread;

    public KeepAliveThread(String servName){
        keepAliveLatch = new CountDownLatch(1);
        thread = new Thread(new Runnable() {

            @Override
            public void run() {
                try {
                    keepAliveLatch.await();
                } catch (InterruptedException e) {

                }

            }
        },"KeepAlive Thread for ====>"+servName);

        thread.setDaemon(false);

        // keep this thread alive (non daemon thread) until we shutdown
        Runtime.getRuntime().addShutdownHook(new Thread() {
            @Override
            public void run() {
                keepAliveLatch.countDown();
                System.out.println("Exit keepalive thread...................................................................");
            }
        });
    }

    public void start(){
        this.thread.start();
    }
}

