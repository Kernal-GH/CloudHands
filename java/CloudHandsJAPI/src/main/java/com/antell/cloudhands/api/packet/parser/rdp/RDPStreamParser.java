package com.antell.cloudhands.api.packet.parser.rdp;

import com.antell.cloudhands.api.packet.ByteData;
import com.antell.cloudhands.api.packet.SessionEntry;
import com.antell.cloudhands.api.packet.parser.StreamParser;
import com.antell.cloudhands.api.packet.parser.StreamParserData;
import com.antell.cloudhands.api.utils.TextUtils;

public class RDPStreamParser implements StreamParser {

    @Override
    public boolean parsable(SessionEntry sessionEntry) {

        if(sessionEntry.getResPort()!=3389)
            return false;

        ByteData byteData = sessionEntry.getReqContent();

        if(byteData == null||byteData.getDataSize()==0)
            return false;

        String text = byteData.decode();

        if(TextUtils.isEmpty(text))
            return false;

        return text.contains("Cookie: mstshash=");
    }

    @Override
    public StreamParserData parse(SessionEntry sessionEntry) {

        String text = sessionEntry.getReqContent().decode();
        int index = text.indexOf("mstshash=");
        int rendIndex = text.indexOf("\r");
        int nendIndex = text.indexOf("\n");

        int endIndex = rendIndex!=-1?rendIndex:nendIndex;

        if(index == -1||endIndex == -1)
            return null;

        String cookie = text.substring(index+9,endIndex);

        return new RDPStreamParserData(cookie);
    }
}
