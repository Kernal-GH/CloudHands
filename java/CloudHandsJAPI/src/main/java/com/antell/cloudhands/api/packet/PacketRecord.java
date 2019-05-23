package com.antell.cloudhands.api.packet;

import com.antell.cloudhands.api.utils.TextUtils;
import com.google.common.io.ByteArrayDataOutput;
import com.google.common.io.ByteStreams;
import org.msgpack.core.MessagePack;
import org.msgpack.core.MessageUnpacker;

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
    public final static int UDPSMON = 8;

    public final static int HTTP = 9;
    public final static int MAIL = 10;
    public final static int FTP = 11;
    public final static int TELNET = 12;
    public final static int TCPSMON = 13;

    public final static int SECRESHTTP = 14;
    public final static int SECRESMAIL = 15;
    public final static int FTPDATA = 16;

    public final static int SSH = 17;

    /*64K*/
    public final static int DATA_SIZE_MIN = 65536;

    public int type;
    public int metaDataSize;
    public int dataSize;
    public long time;

    public ByteBuffer dataBuffer;

    public PacketRecord(long bsize){

        if(bsize<DATA_SIZE_MIN)
            bsize = DATA_SIZE_MIN;

        dataBuffer = ByteBuffer.allocateDirect((int) bsize);

        reset();
    }

    public DataInput getDataInput(){

        byte[] data = new byte[dataSize];

        dataBuffer.get(data);

        return ByteStreams.newDataInput(data);

    }

    public byte[] getData(){

        byte[] data = new byte[dataSize];
        dataBuffer.get(data);

        return data;
    }

    public byte[] getDataWithTypeLen(){

        ByteArrayDataOutput out = ByteStreams.newDataOutput(dataSize+2+8);

        out.writeShort(type);
        out.writeLong(time);

        out.write(getData());

        return out.toByteArray();
    }

    public MessageUnpacker getMessageUnpacker(){

        byte[] data = new byte[dataSize];

        dataBuffer.get(data);

        return MessagePack.newDefaultUnpacker(data);

    }

    public void reset(){

        time = 0;
        metaDataSize = 0;
        dataSize = 0;
        dataBuffer.clear();
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

        return sb.toString();
    }

}
