package com.antell.cloudhands.api.packet.tcp.http;

import com.antell.cloudhands.api.source.SourceEntry;

import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class BasicHttpPartContentParser implements HttpPartContentParser {

    private final Map<String,SourceEntry> partContents;

    public BasicHttpPartContentParser() {

        partContents = new HashMap<>();

    }


    @Override
    public List<SourceEntry> parse(HTTPSession httpSession) {
        return null;
    }

    @Override
    public List<SourceEntry> clean() {
        return null;
    }


}
