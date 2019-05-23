package com.antell.cloudhands.api.packet.parser.mysql;

import com.antell.cloudhands.api.packet.parser.StreamParserData;
import org.elasticsearch.common.xcontent.XContentBuilder;

import java.io.IOException;

public class MySQLStreamParserData implements StreamParserData {

    public static final int LOGIN_OK = 0;
    public static final int LOGIN_FAILED = 1;
    public static final int LOGIN_UNKNOWN= 2;

    private String user;
    private String version;
    private int loginStatus;

    public MySQLStreamParserData(String user){

        this.user = user;
        this.version = "";
        this.loginStatus = LOGIN_UNKNOWN;
    }

    @Override
    public void toJson(XContentBuilder cb) throws IOException {

        cb.field("user",user);
        cb.field("version",version);
        cb.field("loginStatus",loginStatus);

    }


    public String getUser() {
        return user;
    }

    public void setUser(String user) {
        this.user = user;
    }

    public String getVersion() {
        return version;
    }

    public void setVersion(String version) {
        this.version = version;
    }

    public int getLoginStatus() {
        return loginStatus;
    }

    public void setLoginStatus(int loginStatus) {
        this.loginStatus = loginStatus;
    }
}
