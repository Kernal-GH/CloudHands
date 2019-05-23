package com.antell.cloudhands.api.sink.es;

import java.util.List;

public class ESConfigItem {

    private List<String> esHosts;
    private int esPort;
    private String clusterName;
    private String clientType;

    private String indexTypeName;
    private int bulkSize;
    private int threadNum;
    private int queueLimits;

    public ESConfigItem(){

        esPort = 9300;
        clusterName = "elasticsearch";
        clientType = "bulk";
        indexTypeName = "time";
        bulkSize = 1000;
        threadNum = 1;
        queueLimits = 100000;

    }

    public List<String> getEsHosts() {
        return esHosts;
    }

    public void setEsHosts(List<String> esHosts) {
        this.esHosts = esHosts;
    }

    public int getEsPort() {
        return esPort;
    }

    public void setEsPort(int esPort) {
        this.esPort = esPort;
    }

    public String getClusterName() {
        return clusterName;
    }

    public void setClusterName(String clusterName) {
        this.clusterName = clusterName;
    }

    public String getIndexTypeName() {
        return indexTypeName;
    }

    public void setIndexTypeName(String indexTypeName) {
        this.indexTypeName = indexTypeName;
    }

    public int getBulkSize() {
        return bulkSize;
    }

    public void setBulkSize(int bulkSize) {
        this.bulkSize = bulkSize;
    }

    public String getClientType() {
        return clientType;
    }

    public void setClientType(String clientType) {
        this.clientType = clientType;
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
}
