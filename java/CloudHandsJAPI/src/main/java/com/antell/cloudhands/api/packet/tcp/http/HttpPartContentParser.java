package com.antell.cloudhands.api.packet.tcp.http;

import com.antell.cloudhands.api.source.SourceEntry;

import java.util.List;

public interface HttpPartContentParser {

    List<SourceEntry> parse(HTTPSession httpSession);

    List<SourceEntry> clean();

}
