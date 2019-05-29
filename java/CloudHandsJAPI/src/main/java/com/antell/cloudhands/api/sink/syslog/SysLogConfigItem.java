package com.antell.cloudhands.api.sink.syslog;

import java.util.List;

public class SysLogConfigItem {

    private String host;
    private int port;
    private String appName;
    private String devName;
    private int threadNum;
    private int queueLimits;
    private List<String> acceptLogs;

    public String getHost() {
        return host;
    }

    public void setHost(String host) {
        this.host = host;
    }

    public int getPort() {
        return port;
    }

    public void setPort(int port) {
        this.port = port;
    }

    public String getAppName() {
        return appName;
    }

    public void setAppName(String appName) {
        this.appName = appName;
    }

    public String getDevName() {
        return devName;
    }

    public void setDevName(String devName) {
        this.devName = devName;
    }

    public int getThreadNum() {
        return threadNum;
    }

    public void setThreadNum(int threadNum) {
        this.threadNum = threadNum;
    }

    public int getQueueLimits() {
        return queueLimits;
    }

    public void setQueueLimits(int queueLimits) {
        this.queueLimits = queueLimits;
    }

    public List<String> getAcceptLogs() {
        return acceptLogs;
    }

    public void setAcceptLogs(List<String> acceptLogs) {
        this.acceptLogs = acceptLogs;
    }
}
