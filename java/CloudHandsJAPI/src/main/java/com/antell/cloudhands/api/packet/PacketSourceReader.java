package com.antell.cloudhands.api.packet;

import com.antell.cloudhands.api.source.AbstractSourceReader;
import com.antell.cloudhands.api.source.Filter;
import com.antell.cloudhands.api.source.Source;

/**
 * Created by dell on 2018/6/19.
 */
public class PacketSourceReader extends AbstractSourceReader {

    private PacketRecordReader packetRecordReader;

    protected PacketSourceReader(Source source, Filter filter,String mmapFileName) {

        super(source,new PacketSourceEntryParser(), filter);
        this.packetRecordReader = new PacketRecordReader(mmapFileName,null,0,4096);

    }

    @Override
    public int open() {

        return packetRecordReader.open();
    }

    @Override
    public PacketRecord read() {

        return packetRecordReader.read();
    }

    @Override
    public void readEnd(PacketRecord packetRecord) {

        packetRecord.reset();
    }

    @Override
    public void close() {

        packetRecordReader.close();
    }
}
