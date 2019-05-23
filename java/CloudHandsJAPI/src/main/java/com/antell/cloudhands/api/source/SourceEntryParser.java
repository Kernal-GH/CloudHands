package com.antell.cloudhands.api.source;

import com.antell.cloudhands.api.packet.PacketRecord;

import java.io.IOException;
import java.util.List;

/**
 * Created by dell on 2018/6/19.
 */
public interface SourceEntryParser {

    List<SourceEntry> parse(PacketRecord packetRecord) throws IOException;

}
