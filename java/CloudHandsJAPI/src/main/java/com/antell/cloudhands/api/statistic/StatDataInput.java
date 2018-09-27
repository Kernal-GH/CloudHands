package com.antell.cloudhands.api.statistic;

import com.google.common.io.ByteStreams;

import java.io.DataInput;
import java.nio.ByteBuffer;

/**
 * Created by dell on 2018/6/8.
 */
public class StatDataInput {

    public long curDataBufferSize;
    public ByteBuffer dataBuffer;
    public int dataSize;

    public StatDataInput(long bsize){

        curDataBufferSize = bsize;
        dataBuffer = ByteBuffer.allocateDirect((int) bsize);
        reset();
    }

    public DataInput getDataInput(){

        byte[] data = new byte[dataSize];

        dataBuffer.get(data);


        return ByteStreams.newDataInput(data);

    }

    public void reset(){

        dataSize = 0;
        dataBuffer.clear();
    }

    public void updateByteBuffer(long dsize){

        if(dsize>curDataBufferSize){
            dataBuffer = ByteBuffer.allocateDirect((int)dsize+8);
            curDataBufferSize = dsize;
        }
    }

    public void setDataSize(int dataSize) {
        this.dataSize = dataSize;
    }

    public int getDataSize() {
        return dataSize;
    }

}
