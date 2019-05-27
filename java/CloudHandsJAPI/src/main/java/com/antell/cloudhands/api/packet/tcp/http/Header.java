package com.antell.cloudhands.api.packet.tcp.http;

/**
 * Created by dell on 2018/6/11.
 */
public class Header {

    private final String name;
    private final String value;


    public Header(String name, String value) {
        this.name = name;
        this.value = value;
    }

    public String getName() {
        return name;
    }

    public String getValue() {
        return value;
    }

    public String toString(){

        StringBuffer sb = new StringBuffer();
        sb.append(name);
        sb.append(":");
        sb.append(value);

        return sb.toString();
    }
}
