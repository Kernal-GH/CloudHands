package com.antell.cloudhands.api.packet.parser;

import com.antell.cloudhands.api.packet.SessionEntry;
import com.antell.cloudhands.api.packet.parser.pop3.POP3StreamParser;
import com.antell.cloudhands.api.packet.parser.rdp.RDPStreamParser;

public class StreamParserPool {


    private static final StreamParser[] streamParsers = {
            new RDPStreamParser(),
            new POP3StreamParser()
    };

    private StreamParserPool(){

    }


    public static final StreamParserData parse(SessionEntry sessionEntry){

        for(StreamParser parser:streamParsers){

            if(parser.parsable(sessionEntry)){

                return parser.parse(sessionEntry);
            }
        }

        return null;
    }

}
