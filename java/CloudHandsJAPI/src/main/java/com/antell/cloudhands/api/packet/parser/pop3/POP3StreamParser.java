package com.antell.cloudhands.api.packet.parser.pop3;

import com.antell.cloudhands.api.packet.ByteData;
import com.antell.cloudhands.api.packet.SessionEntry;
import com.antell.cloudhands.api.packet.parser.StreamParser;
import com.antell.cloudhands.api.packet.parser.StreamParserData;

public class POP3StreamParser implements StreamParser {


    @Override
    public boolean parsable(SessionEntry sessionEntry) {

        if(sessionEntry.getResPort()!=110)
            return false;

        ByteData content = sessionEntry.getReqContent();
        if(content == null||content.getDataSize()==0||content.getData()==null)
            return false;

        String text = content.decode();

        return text.contains("USER")&&text.contains("PASS");
    }

    @Override
    public StreamParserData parse(SessionEntry sessionEntry) {

        String text = sessionEntry.getReqContent().decode();

        int userIndex = text.indexOf("USER");
        int passIndex = text.indexOf("PASS");

        String user = text.substring(userIndex + 5, passIndex - 2);
        String passStart = text.substring(passIndex + 5);

        String pass = passStart.substring(0, passStart.indexOf("\r"));

        return new POP3StreamParserData(user,pass);
    }


}
