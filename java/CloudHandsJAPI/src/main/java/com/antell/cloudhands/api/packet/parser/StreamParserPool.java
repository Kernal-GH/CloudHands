package com.antell.cloudhands.api.packet.parser;

import com.antell.cloudhands.api.packet.SessionEntry;
import com.antell.cloudhands.api.packet.parser.http.HTTPStreamParser;
import com.antell.cloudhands.api.packet.parser.mssql.MSSQLStreamParser;
import com.antell.cloudhands.api.packet.parser.mysql.MySQLStreamParser;
import com.antell.cloudhands.api.packet.parser.pop3.POP3StreamParser;
import com.antell.cloudhands.api.packet.parser.rdp.RDPStreamParser;
import com.antell.cloudhands.api.packet.parser.redis.RedisStreamParser;
import com.antell.cloudhands.api.packet.parser.smtp.SMTPStreamParser;

public class StreamParserPool {


    private static final StreamParser[] streamParsers = {
            new RDPStreamParser(),
            new POP3StreamParser(),
            new SMTPStreamParser(),
            new HTTPStreamParser(),
            new MySQLStreamParser(),
            new RedisStreamParser(),
            new MSSQLStreamParser()
    };

    private StreamParserPool() {

    }


    public static final StreamParserData parse(SessionEntry sessionEntry) {

        for (StreamParser parser : streamParsers) {

            if (parser.parsable(sessionEntry)) {

                try {

                    return parser.parse(sessionEntry);
                } catch (Exception e) {

                    return null;
                }
            }
        }

        return null;
    }

}
