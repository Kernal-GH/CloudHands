package com.antell.cloudhands.api.packet;

import org.elasticsearch.common.xcontent.XContentBuilder;

import java.io.IOException;

/**
 * Created by dell on 2017/6/28.
 */
public class PortItem {

    private int port;
    private String pprotocol;
    private String protocol;
    private String desc;

    public PortItem(int port,String pprotocol,String protocol,String desc){

        this.port = port;
        this.pprotocol = pprotocol;
        this.protocol = protocol;
        this.desc = desc;
    }

    public int getPort() {
        return port;
    }

    public void setPort(int port) {
        this.port = port;
    }

    public String getPprotocol() {
        return pprotocol;
    }

    public void setPprotocol(String pprotocol) {
        this.pprotocol = pprotocol;
    }

    public String getProtocol() {
        return protocol;
    }

    public void setProtocol(String protocol) {
        this.protocol = protocol;
    }

    public String getDesc() {
        return desc;
    }

    public void setDesc(String desc) {
        this.desc = desc;
    }

    public XContentBuilder toJson(XContentBuilder cb) throws IOException {

        cb.field("pprotocol",pprotocol);
        cb.field("uprotocol",protocol);
        cb.field("desc",desc);

        return cb;
    }
}
