package com.antell.cloudhands.api.packet.parser;

import com.antell.cloudhands.api.packet.SessionEntry;

public interface StreamParser {

    boolean parsable(SessionEntry sessionEntry);
    StreamParserData parse(SessionEntry sessionEntry);


}
