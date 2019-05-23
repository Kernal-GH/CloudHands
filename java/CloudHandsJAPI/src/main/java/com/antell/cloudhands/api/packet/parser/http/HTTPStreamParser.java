package com.antell.cloudhands.api.packet.parser.http;

import com.antell.cloudhands.api.packet.*;
import com.antell.cloudhands.api.packet.parser.StreamParser;
import com.antell.cloudhands.api.packet.parser.StreamParserData;
import com.antell.cloudhands.api.utils.TextUtils;

import java.util.ArrayList;
import java.util.List;

public class HTTPStreamParser implements StreamParser {

    @Override
    public boolean parsable(SessionEntry sessionEntry) {

        PortItem portItem = Portmap.getPortItem((TCPSession) sessionEntry);

        if(!portItem.getProtocol().equals("http"))
            return false;

        ByteData content = sessionEntry.getReqContent();
        if(content == null||content.getDataSize()==0||content.getData()==null)
            return false;

        String text = content.decode();

        if(text.indexOf("\n")==-1)
            return false;

        return true;
    }

    private static final List<String> splits(String line){

        String[] splits = line.split(" ");
        if(splits == null||splits.length<3)
            return null;

        List<String> res = new ArrayList<>();

        for(String item:splits){

            if(item.equals(" "))
                continue;

            res.add(item);
        }

        return res;
    }

    private HTTPStreamParserData parseRequestLine(String line){

        List<String> res = splits(line);

        if(res.size()!=3)
            return null;

        return new HTTPStreamParserData(res.get(0),res.get(1),res.get(2));
    }

    @Override
    public StreamParserData parse(SessionEntry sessionEntry) {

        String reqText = sessionEntry.getReqContent().decode();

        /*parse request line*/

        int start = 0;
        int end;

        int endR = reqText.indexOf('\r');
        int endN = reqText.indexOf('\n');

        end = endR!=-1?endR:endN;

        if(end == -1)
            return null;

        String line = reqText.substring(0,end);

        if(TextUtils.isEmpty(line))
            return null;

        HTTPStreamParserData httpStreamParserData = parseRequestLine(line);

        if(httpStreamParserData == null)
            return null;

        if(reqText.contains("Host:")){

            parseHost(httpStreamParserData,reqText,end);
        }

        ByteData resData = sessionEntry.getResContent();
        if(resData!=null&&resData.getData()!=null&&resData.getData().length>0){

            parseResponseStatusLine(httpStreamParserData,resData.decode());
        }

        return httpStreamParserData;
    }

    private void parseResponseStatusLine(HTTPStreamParserData httpStreamParserData, String text) {

        int status = 0;
        int endR = text.indexOf('\r');
        int endN = text.indexOf('\n');
        int end = endR!=-1?endR:endN;

        if(text.contains("HTTP")&&end!=-1){

            String line = text.substring(0,end);

            List<String> res = splits(line);

            if(res.size()==3){

                try {
                    status = Integer.parseInt(res.get(1));

                }catch (Exception e){

                    status = 0;
                }

                httpStreamParserData.setStatus(status);
            }
        }


    }

    private void parseHost(HTTPStreamParserData httpStreamParserData, String reqText,int start) {

        int startIndex = reqText.indexOf("Host:",start);

        if(startIndex == -1)
            return;

        startIndex+=5;

        int endR = reqText.indexOf('\r',startIndex);
        int endN = reqText.indexOf('\n',startIndex);
        int end = endR!=-1?endR:endR;

        if(end == -1)
            return;

        if(startIndex>=end)
            return;

        httpStreamParserData.setHost(reqText.substring(startIndex,end));

    }

}
