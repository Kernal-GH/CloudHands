package com.antell.cloudhands.api.packet.parser.redis;

import com.antell.cloudhands.api.packet.parser.StreamParserData;
import org.elasticsearch.common.xcontent.XContentBuilder;

import java.io.IOException;

public class RedisStreamParserData implements StreamParserData {


    private String passwd;
    private int isLoginOK;

    public RedisStreamParserData(String passwd,int isLoginOK){

        this.passwd = passwd;
        this.isLoginOK = isLoginOK;
    }

    @Override
    public void toJson(XContentBuilder cb) throws IOException {

        cb.field("passwd",passwd);
        cb.field("isLoginOK",isLoginOK);
    }

    public String getPasswd() {
        return passwd;
    }

    public void setPasswd(String passwd) {
        this.passwd = passwd;
    }

    public int getIsLoginOK() {
        return isLoginOK;
    }

    public void setIsLoginOK(int isLoginOK) {
        this.isLoginOK = isLoginOK;
    }
}
