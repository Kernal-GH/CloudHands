package com.antell.cloudhands.api.packet.tcp.http;

import com.antell.cloudhands.api.packet.tcp.SessionParser;
import com.antell.cloudhands.api.source.SourceEntry;
import org.msgpack.core.MessageUnpacker;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

public class HTTPSessionParser implements SessionParser {

    private final HttpPartContentParser partContentParser;

    public HTTPSessionParser(HttpPartContentParser partContentParser) {
        this.partContentParser = partContentParser;
    }

    @Override
    public List<SourceEntry> parse(MessageUnpacker unpacker) {

        List<SourceEntry> res = new ArrayList<>();
        List<SourceEntry> pres = null;


        try {

            HTTPSession httpSession = new HTTPSession(unpacker);

            if(httpSession.isPartContent()){
                pres = partContentParser.parse(httpSession);
            }else{
                res.add(httpSession);
                pres = partContentParser.clean();
            }
            if(pres!=null&&!pres.isEmpty()){
                res.addAll(pres);
            }

        } catch (IOException e) {

            e.printStackTrace();
        }

        if(res!=null&&!res.isEmpty()){

            List<SourceEntry> gres = new ArrayList<>();
            res.forEach(e->{
                List<SourceEntry> ge = e.generate();
                if(ge!=null&&!ge.isEmpty()){
                    gres.addAll(ge);
                }
            });
            res.addAll(gres);
        }

        return res;

    }


}
