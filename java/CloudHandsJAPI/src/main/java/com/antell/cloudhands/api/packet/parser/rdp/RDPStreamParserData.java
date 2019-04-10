package com.antell.cloudhands.api.packet.parser.rdp;

import com.antell.cloudhands.api.packet.parser.StreamParserData;
import org.elasticsearch.common.xcontent.XContentBuilder;

import java.io.IOException;

public class RDPStreamParserData implements StreamParserData {

    private final String cookie;


    public RDPStreamParserData(String cookie){

        this.cookie = cookie;
    }
    @Override
    public void toJson(XContentBuilder cb) throws IOException {

        cb.field("cookie",cookie);

    }


    public String toString(){

        return cookie;
    }

    public String getCookie() {
        return cookie;
    }

}
