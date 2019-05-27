package com.antell.cloudhands.api.source;

import com.antell.cloudhands.api.packet.PacketRecord;

/**
 * Created by dell on 2018/6/19.
 */
public interface Filter {

    boolean isAccept(PacketRecord packetRecord);

    boolean isAccept(SourceEntry entry);

}
