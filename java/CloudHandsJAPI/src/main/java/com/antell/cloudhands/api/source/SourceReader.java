package com.antell.cloudhands.api.source;

import com.antell.cloudhands.api.packet.PacketRecord;

/**
 * Created by dell on 2018/6/19.
 */
public interface SourceReader {

    int open();

    PacketRecord read();

    void readEnd(PacketRecord packetRecord);

    void start() throws SourceException;

    void stop();

    void close();
}
