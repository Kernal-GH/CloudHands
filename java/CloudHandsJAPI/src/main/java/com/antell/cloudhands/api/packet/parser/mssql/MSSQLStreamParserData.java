package com.antell.cloudhands.api.packet.parser.mssql;

import com.antell.cloudhands.api.packet.parser.StreamParserData;
import org.elasticsearch.common.xcontent.XContentBuilder;

import java.io.IOException;

public class MSSQLStreamParserData implements StreamParserData {

    private String user;
    private String passwd;
    private int isLoginOK;

    public MSSQLStreamParserData(String user,String passwd){

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

    public void setUser(String user) {
        this.user = user;
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
