package com.antell.cloudhands.api.packet.udp.tftp;

import com.antell.cloudhands.api.source.SourceEntry;
import org.msgpack.core.MessageUnpacker;

import java.io.DataInput;
import java.util.List;

public interface TFTPSessionParser {

    List<SourceEntry> parse(DataInput dataInput);
    List<SourceEntry> parse(MessageUnpacker unpacker);
}
