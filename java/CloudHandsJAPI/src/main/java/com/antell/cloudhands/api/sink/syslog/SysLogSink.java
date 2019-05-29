package com.antell.cloudhands.api.sink.syslog;

import com.antell.cloudhands.api.sink.es.Sink;
import com.antell.cloudhands.api.sink.es.SinkException;
import com.cloudbees.syslog.Facility;
import com.cloudbees.syslog.MessageFormat;
import com.cloudbees.syslog.Severity;
import com.cloudbees.syslog.sender.UdpSyslogMessageSender;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.io.IOException;
import java.util.concurrent.ScheduledThreadPoolExecutor;
import java.util.concurrent.TimeUnit;

/**
 * Created with IntelliJ IDEA.
 * Description:
 * User: g
 * Date: 2018-10-23
 * Time: 下午4:34
 */
public class SysLogSink implements Sink {

    private SysLogMessager sysLogMessager = null;
    private final static Logger logger = LoggerFactory.getLogger(SysLogSink.class);
    private SysLogSinkQueue sinkQueue;

    private ScheduledThreadPoolExecutor scheduledThreadPoolExecutor;
    private SysLogConfigItem sysLogConfigItem;

    public SysLogSink(SysLogConfigItem sysLogConfigItem, SysLogSinkQueue sinkQueue) {

        this.sysLogConfigItem = sysLogConfigItem;
        this.sinkQueue = sinkQueue;

        sysLogMessager = new SysLogMessager();
    }

    @Override
    public void start() throws SinkException {

        scheduledThreadPoolExecutor = new ScheduledThreadPoolExecutor(sysLogConfigItem.getThreadNum());

        for (int i = 0; i < sysLogConfigItem.getThreadNum(); i++) {
            scheduledThreadPoolExecutor.scheduleAtFixedRate(new SysLogSinkRunnable(), 500, 500, TimeUnit.MILLISECONDS);
        }

    }

    @Override
    public void stop() {

    }

    private class SysLogSinkRunnable implements Runnable {

        public SysLogSinkRunnable() {
        }

        @Override
        public void run() {
            while (true) {

                try {

                    String message = (String) sinkQueue.take();

                    if (message == null) {
                        break;
                    }

                    sysLogMessager.send(message);

                } catch (Exception e) {
                    e.printStackTrace();
                }
            }

        }
    }

    private class SysLogMessager {
        private UdpSyslogMessageSender messageSender = null;

        public SysLogMessager() {

            this.messageSender = new UdpSyslogMessageSender();
            this.messageSender.setDefaultMessageHostname(sysLogConfigItem.getDevName()); // some syslog cloud services may use this field to transmit a secret key
            this.messageSender.setDefaultAppName(sysLogConfigItem.getAppName());
            this.messageSender.setDefaultFacility(Facility.USER);
            this.messageSender.setDefaultSeverity(Severity.INFORMATIONAL);
            this.messageSender.setSyslogServerHostname(sysLogConfigItem.getHost());
// syslog udp usually uses port 514 as per https://tools.ietf.org/html/rfc3164#page-5
            this.messageSender.setSyslogServerPort(sysLogConfigItem.getPort());
            this.messageSender.setMessageFormat(MessageFormat.RFC_3164); // optional, default is RFC 3164

        }

        public int send(String msg) {

            try {
                this.messageSender.sendMessage(msg);
                //System.out.println("send message: " + msg);
            } catch (IOException e) {
                e.printStackTrace();
                return -1;
            }

            return 0;
        }

    }
}
