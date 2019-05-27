package com.antell.cloudhands.api.packet.parser.mssql;

import com.antell.cloudhands.api.packet.ByteData;
import com.antell.cloudhands.api.packet.SessionEntry;
import com.antell.cloudhands.api.packet.parser.StreamParser;
import com.antell.cloudhands.api.packet.parser.StreamParserData;

public class MSSQLStreamParser implements StreamParser {

    @Override
    public boolean parsable(SessionEntry sessionEntry) {

        if(sessionEntry.getResPort()!=1433)
            return false;

        ByteData data = sessionEntry.getReqContent();

        if(data == null ||data.getDataSize()<101||data.getData()==null)
            return false;

        byte[] content = data.getData();

        if((content[0]&0xff)!=0x02)
            return false;

        int userOffset = 39;
        int userLen = content[69]&0xff;
        int passwdOffset = 70;
        int passwdLen = content[100]&0xff;

        return isValid(userOffset,userLen,content.length)&&isValid(passwdOffset,passwdLen,content.length);
    }

    private static final boolean isValid(int offset,int len,int dlen){

        return offset+len<=dlen;
    }

    private MSSQLStreamParserData parseRequest(SessionEntry sessionEntry){

        byte[] content = sessionEntry.getReqContent().getData();

        int userOffset = 39;
        int userLen = content[69]&0xff;
        int passwdOffset = 70;
        int passwdLen = content[100]&0xff;

        return new MSSQLStreamParserData(new String(content,userOffset,userLen),new String(content,passwdOffset,passwdLen));

    }

    private void parseResponse(SessionEntry sessionEntry,MSSQLStreamParserData parserData){

        ByteData content = sessionEntry.getResContent();
        if(content == null||content.getDataSize()<9||(content.getData()[0]&0xff)!=0x04)
            return;

        parserData.setIsLoginOK((content.getData()[8]&0xff)==0xaa?0:1);
    }

    @Override
    public StreamParserData parse(SessionEntry sessionEntry) {

        MSSQLStreamParserData parserData = parseRequest(sessionEntry);

        parseResponse(sessionEntry,parserData);

        return parserData;
    }

}
