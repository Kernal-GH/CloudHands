package com.antell.cloudhands.api.packet;

import com.antell.cloudhands.api.utils.TextUtils;

import java.io.DataInput;
import java.io.IOException;

/**
 * Created by dell on 2018/4/16.
 */
public class TCPSession extends PacketSessionEntry{

    private int phaseState;


    public int getPhaseState() {
        return phaseState;
    }

    public void setPhaseState(int phaseState) {
        this.phaseState = phaseState;
    }

    @Override
    public void read(DataInput input) throws IOException {

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

        ByteData reqContent = new ByteData();
        reqContent.read(input);

        ByteData resContent = new ByteData();
        resContent.read(input);

        setReqContent(reqContent);
        setResContent(resContent);

    }

    @Override
    public String toString(){

        StringBuffer sb = new StringBuffer();
        sb.append("Dump TCP Packet Session informations:\n");
        TextUtils.addInt(sb,"PhaseState",phaseState);

        super.dump(sb);

        return sb.toString();
    }

}
