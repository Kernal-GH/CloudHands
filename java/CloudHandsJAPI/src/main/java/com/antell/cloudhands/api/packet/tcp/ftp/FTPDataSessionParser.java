package com.antell.cloudhands.api.packet.tcp.ftp;

import com.antell.cloudhands.api.packet.tcp.SessionParser;
import com.antell.cloudhands.api.source.SourceEntry;
import org.msgpack.core.MessageUnpacker;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

public class FTPDataSessionParser implements SessionParser {


    @Override
    public List<SourceEntry> parse(MessageUnpacker unpacker) {

        List<SourceEntry> res = new ArrayList<>();

        try {

            FTPDataSession dataSession = new FTPDataSession(unpacker);
           // res.add(dataSession);
            List<SourceEntry> sourceEntries = dataSession.generate();

            if(sourceEntries!=null&&!sourceEntries.isEmpty())
                res.addAll(sourceEntries);

        } catch (IOException e) {
            e.printStackTrace();
        }

        return res;
    }

}
