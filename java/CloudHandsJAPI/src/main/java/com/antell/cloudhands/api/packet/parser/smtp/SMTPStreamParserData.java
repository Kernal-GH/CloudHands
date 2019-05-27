package com.antell.cloudhands.api.packet.parser.smtp;

import com.antell.cloudhands.api.packet.parser.StreamParserData;
import org.elasticsearch.common.xcontent.XContentBuilder;

import java.io.IOException;

public class SMTPStreamParserData implements StreamParserData {

    private String user;
    private String passwd;
    private String from;
    private int isLoginOK;

    public SMTPStreamParserData(String user,String passwd,String from){

        this.user = user;
        this.passwd = passwd;
        this.from = from;
        this.isLoginOK = 0;
    }

    @Override
    public void toJson(XContentBuilder cb) throws IOException {

        cb.field("user",user);
        cb.field("passwd",passwd);
        cb.field("from",from);
        cb.field("isLoginOK",isLoginOK);

    }

    public String getUser() {
        return user;
    }

    public void setUser(String user) {
        this.user = user;
    }

    public String getPasswd() {
        return passwd;
    }

    public void setPasswd(String passwd) {
        this.passwd = passwd;
    }

    public String getFrom() {
        return from;
    }

    public void setFrom(String from) {
        this.from = from;
    }



    public int getIsLoginOK() {
        return isLoginOK;
    }

    public void setIsLoginOK(int isLoginOK) {
        this.isLoginOK = isLoginOK;
    }
}
