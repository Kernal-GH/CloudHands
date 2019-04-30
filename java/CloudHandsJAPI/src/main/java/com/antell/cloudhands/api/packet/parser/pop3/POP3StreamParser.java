package com.antell.cloudhands.api.packet.parser.pop3;

import com.antell.cloudhands.api.packet.ByteData;
import com.antell.cloudhands.api.packet.SessionEntry;
import com.antell.cloudhands.api.packet.parser.LineTextReader;
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

    private POP3StreamParserData parseRequest(SessionEntry sessionEntry){

        byte[] data = sessionEntry.getReqContent().getData();

        LineTextReader reader = new LineTextReader(data,0,data.length);
        String user = "";
        String passwd = "";
        while(reader.hasLine()){

            String line = reader.readLine();
            if(line.startsWith("USER")){

                String[] splits = line.split(" ");
                if(splits.length >2){
                    user = splits[1];
                }
            }else if(line.startsWith("PASS")){
                String[] splits = line.split(" ");
                if(splits.length >2){
                    passwd = splits[1];
                }
            }
        }

        return new POP3StreamParserData(user,passwd);
    }

    private void parseResponse(SessionEntry sessionEntry,POP3StreamParserData parserData){

        ByteData content = sessionEntry.getResContent();
        if(content == null || content.getData()==null ||content.getDataSize()<3)
            return;

        String text = content.decode();

        if(text.startsWith("+OK"))
            parserData.setIsLoginOK(1);


    }

    @Override
    public StreamParserData parse(SessionEntry sessionEntry) {

        POP3StreamParserData parserData = parseRequest(sessionEntry);
        if(parserData!=null){
            parseResponse(sessionEntry,parserData);

        }

        return parserData;
    }


}
