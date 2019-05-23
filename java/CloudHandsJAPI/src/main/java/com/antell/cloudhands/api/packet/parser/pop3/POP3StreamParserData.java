package com.antell.cloudhands.api.packet.parser.pop3;

import com.antell.cloudhands.api.packet.parser.StreamParserData;
import org.elasticsearch.common.xcontent.XContentBuilder;

import java.io.IOException;

public class POP3StreamParserData implements StreamParserData {

    private final String user;
    private final String passwd;
    private int isLoginOK;

    public POP3StreamParserData(String user, String passwd) {
        this.user = user;
        this.passwd = passwd;
        this.isLoginOK = 0;
    }

    @Override
    public void toJson(XContentBuilder cb) throws IOException {

        cb.field("user",user);
        cb.field("passwd",passwd);
        cb.field("isLoginOK",isLoginOK);

    }


    public String getUser() {
        return user;
    }

    public String getPasswd() {
        return passwd;
    }

    public int getIsLoginOK() {
        return isLoginOK;
    }

    public void setIsLoginOK(int isLoginOK) {
        this.isLoginOK = isLoginOK;
    }
}
