package com.antell.cloudhands.api.packet.parser.redis;

import com.antell.cloudhands.api.packet.ByteData;
import com.antell.cloudhands.api.packet.SessionEntry;
import com.antell.cloudhands.api.packet.parser.StreamParser;
import com.antell.cloudhands.api.packet.parser.StreamParserData;
import com.antell.cloudhands.api.utils.TextUtils;

public class RedisStreamParser implements StreamParser {

    @Override
    public boolean parsable(SessionEntry sessionEntry) {

        if(sessionEntry.getResPort()!=6379)
            return false;

        ByteData content = sessionEntry.getReqContent();
        if(content == null||content.getDataSize()==0||content.getData()==null)
            return false;

        String text = content.decode();

        return text.contains("AUTH");

    }

    private RedisStreamParserData parseRequest(SessionEntry sessionEntry){

        String text = sessionEntry.getReqContent().decode();


        int len = text.length();
        int begin = text.indexOf("AUTH")+4;
        int end = begin;

        if(begin>=len)
            return null;

        if(text.charAt(begin)==' '){

            begin = begin+1;
            end = TextUtils.getLineIndex(text,begin);

        }else if(text.charAt(begin)=='\r'||text.charAt(begin)=='\n'){

            begin = text.indexOf('$',begin);
            if(begin==-1||begin+1>=len)
                return null;
            begin+=1;

            begin = TextUtils.getLineIndex(text,begin);
            if(begin == -1||begin>=len)
                return null;

            if(text.charAt(begin)=='\r')
                begin+=2;
            else
                begin+=1;

            end = TextUtils.getLineIndex(text,begin);

        }else {

            return null;
        }

        if(begin>=end||end>len)
            return null;

        return new RedisStreamParserData(text.substring(begin,end),0);
    }

    private void parseResponse(SessionEntry sessionEntry,RedisStreamParserData parserData){

        ByteData content = sessionEntry.getResContent();
        if(content == null ||content.getData()==null||content.getDataSize()==0)
            return;

        String text = content.decode();
        if(text.startsWith("+OK"))
            parserData.setIsLoginOK(1);
        else
            parserData.setIsLoginOK(0);


    }

    @Override
    public StreamParserData parse(SessionEntry sessionEntry) {

        RedisStreamParserData parserData = parseRequest(sessionEntry);
        if(parserData == null)
            return parserData;

        parseResponse(sessionEntry,parserData);

        return parserData;
    }


}
