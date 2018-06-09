package com.antell.cloudhands.api.statistic;

import com.antell.cloudhands.api.utils.TextUtils;

import java.io.DataInput;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

/**
 * Created by dell on 2018/6/8.
 */
public class StatDataResult implements DataResultBuilder{

    private long startTime;
    private long endTime;
    private long tv;

    private long packets;
    private long bytes;

    private List<StatDataObj> statDataObjs;

    public StatDataResult(){

        startTime = 0;
        endTime = 0;
        tv = 0;
        packets = 0;
        bytes = 0;

        statDataObjs = new ArrayList<>();
    }

    public long getStartTime() {
        return startTime;
    }

    public void setStartTime(long startTime) {
        this.startTime = startTime;
    }

    public long getEndTime() {
        return endTime;
    }

    public void setEndTime(long endTime) {
        this.endTime = endTime;
    }

    public long getTv() {
        return tv;
    }

    public void setTv(long tv) {
        this.tv = tv;
    }

    public long getPackets() {
        return packets;
    }

    public void setPackets(long packets) {
        this.packets = packets;
    }

    public long getBytes() {
        return bytes;
    }

    public void setBytes(long bytes) {
        this.bytes = bytes;
    }

    @Override
    public void build(DataInput din) throws IOException {

        setEndTime(din.readLong());
        setStartTime(din.readLong());
        din.readLong();/*up time*/
        setTv(din.readLong());
        setPackets(din.readLong());
        setBytes(din.readLong());

        /*read obj number*/
        int n = din.readUnsignedShort();

        for(int i = 0;i<n;i++){

            StatDataObj obj = new StatDataObj();
            obj.build(din,startTime,tv);
            statDataObjs.add(obj);
        }
    }

    @Override
    public String toString(){

        StringBuffer sb = new StringBuffer();
        TextUtils.addLong(sb,"startTime",startTime);
        TextUtils.addLong(sb,"endTime",endTime);
        TextUtils.addLong(sb,"tv",tv);

        TextUtils.addLong(sb,"packets",packets);
        TextUtils.addLong(sb,"bytes",bytes);

        for(StatDataObj obj: statDataObjs){

            obj.dump(sb);
        }

        return sb.toString();
    }

}
