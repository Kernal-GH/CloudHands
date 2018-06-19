package com.antell.cloudhands.api.source;

import com.antell.cloudhands.api.packet.PacketRecord;

import java.io.IOException;

/**
 * Created by dell on 2018/6/19.
 */
public interface SourceEntryParser {

    SourceEntry parse(PacketRecord packetRecord) throws IOException;

}
