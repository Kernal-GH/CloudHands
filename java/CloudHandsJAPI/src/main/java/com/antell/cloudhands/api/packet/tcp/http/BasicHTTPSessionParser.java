package com.antell.cloudhands.api.packet.tcp.http;

import com.antell.cloudhands.api.source.SourceEntry;
import org.msgpack.core.MessageUnpacker;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

public class BasicHTTPSessionParser implements HTTPSessionParser{

    private final HttpPartContentParser partContentParser;

    public BasicHTTPSessionParser(HttpPartContentParser partContentParser) {
        this.partContentParser = partContentParser;
    }

    @Override
    public List<SourceEntry> parse(MessageUnpacker unpacker) {

        List<SourceEntry> res = new ArrayList<>();
        List<SourceEntry> pres = null;

        try {

            HTTPSession httpSession = new HTTPSession(unpacker);

            if(httpSession.getStatus() == 206){
                pres = partContentParser.parse(httpSession);
            }else{
                res.add(httpSession);
                pres = partContentParser.clean();
            }
            if(pres!=null&&!pres.isEmpty()){
                res.addAll(pres);
            }

        } catch (IOException e) {

        }

        return res;

    }


}
