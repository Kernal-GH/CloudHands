package com.antell.cloudhands.api.packet;

import com.antell.cloudhands.api.BinDataInput;
import com.antell.cloudhands.api.source.SourceEntry;
import com.antell.cloudhands.api.utils.DateUtils;
import com.antell.cloudhands.api.utils.IPUtils;
import com.antell.cloudhands.api.utils.TextUtils;
import org.elasticsearch.common.xcontent.XContentBuilder;

import java.io.DataInput;
import java.io.IOException;

/**
 * Created by dell on 2018/4/16.
 */
public class TCPSession extends SessionEntry implements SourceEntry{

    private int phaseState;


    public int getPhaseState() {
        return phaseState;
    }

    public void setPhaseState(int phaseState) {
        this.phaseState = phaseState;
    }

    public TCPSession(DataInput input) throws IOException {

        setTimeout(input.readUnsignedByte()!=0);
        setTimeoutTV(input.readUnsignedShort());
        setPhaseState(input.readUnsignedShort());
        setReqPort(input.readUnsignedShort());
        setResPort(input.readUnsignedShort());
        setReqIP((long)input.readInt());
        setResIP((long)input.readInt());
        setSessionID(input.readLong());
        setReqPackets(input.readLong());
        setResPackets(input.readLong());
        setReqBytes(input.readLong());
        setResBytes(input.readLong());
        setReqStartTime(input.readLong());
        setReqLastTime(input.readLong());
        setResStartTime(input.readLong());
        setResLastTime(input.readLong());

        ByteData reqContent = new ByteData(input);
        ByteData resContent = new ByteData(input);

        setReqContent(reqContent);
        setResContent(resContent);

    }



    @Override
    public XContentBuilder dataToJson(XContentBuilder cb) throws IOException {

        cb.field("proto","tcp");
        cb.field("srcIP", IPUtils.ipv4Str(getReqIP()));
        cb.field("dstIP",IPUtils.ipv4Str(getResIP()));
        cb.field("srcPort",getReqPort());
        cb.field("dstPort",getResPort());
        cb.field("phaseState",getPhaseState());
        cb.field("sessionID",getSessionID());
        cb.field("reqPackets",getReqPackets());
        cb.field("reqBytes",getReqBytes());
        cb.field("resPackets",getResPackets());
        cb.field("resBytes",getResBytes());

        ByteData reqContent = getReqContent();
        cb.field("reqDataSize",reqContent!=null?reqContent.getDataSize():0);
        cb.field("reqData",reqContent!=null?reqContent.getData():"".getBytes());

        ByteData resContent = getResContent();
        cb.field("resDataSize",resContent!=null?resContent.getDataSize():0);
        cb.field("resData",resContent!=null?resContent.getData():"".getBytes());

        cb.field("reqStartTime",getReqStartTime());
        cb.field("reqLastTime",getReqLastTime());
        cb.field("resStartTime",getResStartTime());
        cb.field("resLastTime",getResLastTime());
        cb.field("timeDate", DateUtils.format(getReqStartTime()));

        return cb;
    }

    @Override
    public String dataToString() {

        StringBuffer sb = new StringBuffer();
        sb.append("Dump TCP Packet Session informations:\n");
        TextUtils.addInt(sb,"PhaseState",phaseState);

        sb.append(entryToString());

        return sb.toString();
    }

    @Override
    public String toString(){

        return dataToString();
    }
}
