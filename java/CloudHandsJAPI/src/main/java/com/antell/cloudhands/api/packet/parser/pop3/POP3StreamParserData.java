package com.antell.cloudhands.api.packet.parser.pop3;

import com.antell.cloudhands.api.packet.parser.StreamParserData;
import org.elasticsearch.common.xcontent.XContentBuilder;

import java.io.IOException;

public class POP3StreamParserData implements StreamParserData {

    private final String user;
    private final String passwd;

    public POP3StreamParserData(String user, String passwd) {
        this.user = user;
        this.passwd = passwd;
    }

    @Override
    public void toJson(XContentBuilder cb) throws IOException {

        cb.field("user",user);
        cb.field("passwd",passwd);

    }


    public String getUser() {
        return user;
    }

    public String getPasswd() {
        return passwd;
    }
}
