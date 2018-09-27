package com.antell.cloudhands.api.packet.tcp;

import com.antell.cloudhands.api.source.SourceEntry;
import org.msgpack.core.MessageUnpacker;

import java.util.List;

public interface SessionParser {

    List<SourceEntry> parse(MessageUnpacker unpacker);
}
