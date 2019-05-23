package com.antell.cloudhands.api.packet.parser.smtp;

import com.antell.cloudhands.api.packet.ByteData;
import com.antell.cloudhands.api.packet.SessionEntry;
import com.antell.cloudhands.api.packet.parser.LineTextReader;
import com.antell.cloudhands.api.packet.parser.StreamParser;
import com.antell.cloudhands.api.packet.parser.StreamParserData;
import com.antell.cloudhands.api.utils.TextUtils;

public class SMTPStreamParser implements StreamParser {


    private final static boolean isAuthPlain(String text){

        int index = text.indexOf("AUTH PLAIN");
        if(index == -1)
            return false;

        return TextUtils.getLineIndex(text,index)!=-1;
    }

    public final static boolean isAuthLogin(String text){

        int index = text.indexOf("AUTH LOGIN");
        if(index == -1)
            return false;

        return TextUtils.getLineIndex(text,index+12)!=-1;
    }

    @Override
    public boolean parsable(SessionEntry sessionEntry) {

        if(sessionEntry.getResPort()!=25)
            return false;

        ByteData content = sessionEntry.getReqContent();
        if(content == null||content.getDataSize()<10||content.getData()==null)
            return false;

        String text = content.decode();

        if(TextUtils.isEmpty(text))
            return false;


        return isAuthPlain(text)||isAuthLogin(text);
    }

    private SMTPStreamParserData parseAuthPlain(SessionEntry sessionEntry){

        byte[] content = sessionEntry.getReqContent().getData();

        LineTextReader reader = new LineTextReader(content,0,content.length);

        String user = "";
        String passwd = "";
        String mailFrom = "";

        while(reader.hasLine()){

            String line = reader.readLine();
            if(TextUtils.isEmpty(line))
                continue;

            if(line.startsWith("AUTH PLAIN")){

                String[] splits = line.split(" ");
                if(splits.length!=3)
                    return null;
                String text = TextUtils.decodeBase64(splits[2]);
                if(TextUtils.isEmpty(text))
                    return null;

                splits = text.split(" ");
                if(splits.length!=2)
                    return null;

                user = splits[0];
                passwd = splits[1];
                break;
            }
        }

        String contentText = sessionEntry.getReqContent().decode();
        int start = contentText.indexOf("mail FROM:");

        if(start!=-1){

            int end = contentText.indexOf(" ",start+10);
            if(end!=-1){

                mailFrom = contentText.substring(start+10,end);
            }
        }

        return new SMTPStreamParserData(user,passwd,mailFrom);

    }

    private SMTPStreamParserData parseAuthLogin(SessionEntry sessionEntry){

        byte[] content = sessionEntry.getReqContent().getData();

        LineTextReader reader = new LineTextReader(content,0,content.length);

        String user = "";
        String passwd = "";
        String mailFrom = "";

        while(reader.hasLine()){

            String line = reader.readLine();
            if(TextUtils.isEmpty(line))
                continue;

            if(line.startsWith("AUTH LOGIN")){

                if(reader.hasLine())
                    user = TextUtils.decodeBase64(reader.readLine());
                if(reader.hasLine())
                    passwd = TextUtils.decodeBase64(reader.readLine());
            }else if(line.startsWith("MAIL FROM")){

                if(reader.hasLine())
                    mailFrom = reader.readLine();
            }
        }


        return new SMTPStreamParserData(user,passwd,mailFrom);
    }


    private SMTPStreamParserData parseRequest(SessionEntry sessionEntry){


        if(isAuthPlain(sessionEntry.getReqContent().decode()))
            return parseAuthPlain(sessionEntry);

        return parseAuthLogin(sessionEntry);
    }

    private void parseResponse(SessionEntry sessionEntry, SMTPStreamParserData parserData) {

        ByteData content = sessionEntry.getResContent();

        if(content == null || content.getDataSize()<10||content.getData()==null)
            return;

        String text = content.decode();

        if(text.contains("250-AUTH"))
            parserData.setIsLoginOK(1);


    }

    @Override
    public StreamParserData parse(SessionEntry sessionEntry) {

        SMTPStreamParserData parserData = parseRequest(sessionEntry);
        if(parserData!=null){

            parseResponse(sessionEntry,parserData);
        }
        return parserData;
    }

}
