package com.antell.cloudhands.api.packet.udp.tftp;

import com.antell.cloudhands.api.source.SourceEntry;
import org.msgpack.core.MessageUnpacker;

import java.io.DataInput;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

public class BasicTFTPSessionParser implements TFTPSessionParser {

    @Override
    public List<SourceEntry> parse(DataInput dataInput) {

        List<SourceEntry> res = new ArrayList<>();

        try {

            TFTPSession tftpSession = new TFTPSession(dataInput);

            res.add(tftpSession);

            List<SourceEntry> sourceEntries = tftpSession.generate();

            if (sourceEntries != null && !sourceEntries.isEmpty())
                res.addAll(sourceEntries);

        } catch (IOException e) {
            e.printStackTrace();
        }

        return res;
    }

    @Override
    public List<SourceEntry> parse(MessageUnpacker unpacker) {
        List<SourceEntry> res = new ArrayList<>();

        try {

            TFTPSession tftpSession = new TFTPSession(unpacker);

            res.add(tftpSession);

            List<SourceEntry> sourceEntries = tftpSession.generate();

            if (sourceEntries != null && !sourceEntries.isEmpty())
                res.addAll(sourceEntries);

        } catch (IOException e) {
            e.printStackTrace();
        }

        return res;
    }

}
