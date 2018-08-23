package com.antell.cloudhands.api.packet.tcp.http;

import com.antell.cloudhands.api.source.SourceEntry;
import org.msgpack.core.MessageUnpacker;

import java.util.List;

public interface HTTPSessionParser {

    List<SourceEntry> parse(MessageUnpacker unpacker);

}
