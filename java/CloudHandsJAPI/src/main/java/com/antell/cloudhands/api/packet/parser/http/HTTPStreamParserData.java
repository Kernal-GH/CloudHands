package com.antell.cloudhands.api.packet.parser.http;

import com.antell.cloudhands.api.packet.parser.StreamParserData;
import org.elasticsearch.common.xcontent.XContentBuilder;

import java.io.IOException;

public class HTTPStreamParserData implements StreamParserData {

    private String method;
    private String uri;
    private String version;
    private String host;
    private int status;

    public HTTPStreamParserData(String method,String uri,String version){

        this.method = method;
        this.uri = uri;
        this.version = version;
        this.host = "";
        this.status = 0;
    }

    @Override
    public void toJson(XContentBuilder cb) throws IOException {

        cb.field("method",method);
        cb.field("uri",uri);
        cb.field("version",version);
        cb.field("host",host);
        cb.field("status",status);
    }


    public String getMethod() {
        return method;
    }

    public void setMethod(String method) {
        this.method = method;
    }

    public String getUri() {
        return uri;
    }

    public void setUri(String uri) {
        this.uri = uri;
    }

    public String getVersion() {
        return version;
    }

    public void setVersion(String version) {
        this.version = version;
    }

    public String getHost() {
        return host;
    }

    public void setHost(String host) {
        this.host = host;
    }

    public int getStatus() {
        return status;
    }

    public void setStatus(int status) {
        this.status = status;
    }
}
