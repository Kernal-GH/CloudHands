package com.antell.cloudhands.api.packet;

import com.antell.cloudhands.api.BinDataInput;
import org.msgpack.core.MessageUnpacker;

import java.io.DataInput;
import java.io.IOException;

/**
 * Created by dell on 2018/6/9.
 */
public class ByteData implements BinDataInput{

    private long dataSize;
    private byte[] data;

    public ByteData(){

        dataSize = 0;
        data = null;
    }

    @Override
    public void read(DataInput in) throws IOException {

        dataSize = in.readLong();

        this.data = new byte[(int)dataSize];

        in.readFully(data,0,(int)dataSize);

    }

    public long getDataSize() {
        return dataSize;
    }

    public byte[] getData() {
        return data;
    }

}
