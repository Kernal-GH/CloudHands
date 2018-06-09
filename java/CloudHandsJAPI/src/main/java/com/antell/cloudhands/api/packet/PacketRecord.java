package com.antell.cloudhands.api.packet;

import com.antell.cloudhands.api.utils.TextUtils;
import com.google.common.io.ByteStreams;

import java.io.DataInput;
import java.nio.ByteBuffer;

/**
 * Created by dell on 2018/6/9.
 */
public class PacketRecord {

    public final static int TCP = 1;
    public final static int UDP = 2;
    public final static int ICMP = 3;
    public final static int ARP = 4;
    public final static int OTHER = 5;
    public final static int DNS = 6;
    public final static int TFTP = 7;
    public final static int HTTP = 8;
    public final static int MAIL = 9;
    public final static int FTP = 10;
    public final static int TELNET = 11;
    public final static int SECRES = 12;

    public int type;
    public int metaDataSize;
    public int dataSize;
    public long time;

    public long curDataBufferSize;
    public ByteBuffer dataBuffer;

    public PacketRecord(long bsize){

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

        time = 0;
        metaDataSize = 0;
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

    public int getType(){

        return type;

    }

    public void setType(int type){

        this.type = type;
    }

    public int getMetaDataSize() {
        return metaDataSize;
    }

    public void setMetaDataSize(int metaDataSize) {
        this.metaDataSize = metaDataSize;
    }

    public long getTime() {
        return time;
    }

    public void setTime(long time) {
        this.time = time;
    }

    @Override
    public String toString(){

        StringBuffer sb = new StringBuffer();

        TextUtils.addInt(sb,"type",type);
        TextUtils.addInt(sb,"dataSize",dataSize);
        TextUtils.addInt(sb,"metaDataSize",metaDataSize);
        TextUtils.addLong(sb,"time",time);
        TextUtils.addLong(sb,"curDataBufferSize",curDataBufferSize);

        return sb.toString();
    }

}
