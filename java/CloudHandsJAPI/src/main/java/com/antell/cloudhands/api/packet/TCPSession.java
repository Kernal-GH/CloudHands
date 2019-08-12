package com.antell.cloudhands.api.packet;

import com.antell.cloudhands.api.packet.parser.StreamParserData;
import com.antell.cloudhands.api.packet.parser.StreamParserPool;
import com.antell.cloudhands.api.source.SourceEntry;
import com.antell.cloudhands.api.utils.*;
import org.elasticsearch.common.xcontent.XContentBuilder;
import org.msgpack.core.MessageUnpacker;

import java.io.DataInput;
import java.io.IOException;

/**
 * Created by dell on 2018/4/16.
 */
public class TCPSession extends SessionEntry implements SourceEntry{

    private int phaseState;

    private final String objectId;


    public int getPhaseState() {
        return phaseState;
    }

    public void setPhaseState(int phaseState) {
        this.phaseState = phaseState;
    }

    public TCPSession(DataInput input) throws IOException {
        this.objectId = TextUtils.getUUID();
        setTimeout(input.readUnsignedByte()!=0);
        setTimeoutTV(input.readUnsignedShort());
        setPhaseState(input.readUnsignedShort());
        setReqPort(input.readUnsignedShort());
        setResPort(input.readUnsignedShort());
        setIPV6(input.readUnsignedByte()==1);
        if(isIPV6()){
            setReqAddr(readBytes(input));
            setResAddr(readBytes(input));
        }else{
            setReqIP((long)input.readInt());
            setResIP((long)input.readInt());
        }

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


        setPortScan();
        setSdp();
    }


    @Override
    public String getObjectId(){

        return objectId;
    }

    public String getProto(){

        return "tcp";
    }

    public long getSrcIPI(){

        return getReqIP();
    }

    public long getDstIPI(){
        return getResIP();
    }

    public long getTime(){

        return getReqStartTime();
    }

    @Override
    public XContentBuilder dataToJson(XContentBuilder cb) throws IOException {

        cb.field("objectId",objectId);
        cb.field("proto","tcp");
        cb.field("isIPV6",isIPV6()?1:0);
        cb.field("srcIP", getsrcIPStr());
        cb.field("dstIP",getdstIPStr());
        cb.field("srcPort",getReqPort());
        cb.field("dstPort",getResPort());
        cb.field("phaseState",getPhaseState());
        cb.field("sessionID",getSessionID());
        cb.field("bytes",getReqBytes()+getResBytes());
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
        cb.field("portScan",getPortScan());
        cb.field("sdp",getSdp());

        XContentBuilder cbb = cb.startObject("app");

        PortItem portItem = Portmap.getPortItem(this);
        portItem.toJson(cbb);
        cbb.endObject();

        appendParserData(cb);

        return cb;
    }

    @Override
    public String getIndexMapping() {

        String mapping = "{" +
                "\"properties\":{" +
                "\"id\":{\"type\":\"keyword\"}," +
                "\"objectId\":{\"type\":\"keyword\"}," +
                "\"proto\":{\"type\":\"keyword\"}," +
                "\"portScan\":{\"type\":\"keyword\"}," +
                "\"sdp\":{\"type\":\"keyword\"}," +
                "\"srcIP\":{\"type\":\"keyword\"}," +
                "\"dstIP\":{\"type\":\"keyword\"}," +
                "\"srcPort\":{\"type\":\"integer\"}," +
                "\"dstPort\":{\"type\":\"integer\"}," +
                "\"phaseState\":{\"type\":\"integer\"}," +
                "\"sessionID\":{\"type\":\"long\"}," +
                "\"bytes\":{\"type\":\"long\"}," +
                "\"reqPackets\":{\"type\":\"long\"}," +
                "\"reqBytes\":{\"type\":\"long\"}," +
                "\"resPackets\":{\"type\":\"long\"}," +
                "\"resBytes\":{\"type\":\"long\"}," +
                "\"reqDateSize\":{\"type\":\"long\"}," +
                "\"reqData\":{\"type\":\"binary\"}," +
                "\"resDateSize\":{\"type\":\"long\"}," +
                "\"resData\":{\"type\":\"binary\"}," +
                "\"reqStartTime\":{\"type\":\"long\"}," +
                "\"reqLastTime\":{\"type\":\"long\"}," +
                "\"resStartTime\":{\"type\":\"long\"}," +
                "\"resLastTime\":{\"type\":\"long\"}," +
                "\"timeDate\":{\"type\":\"date\",\"format\":\"yyyy-MM-dd HH:mm:ss\"}," +
                "\"app\":{" +
                "\"properties\":{" +
                "\"pprotocol\":{\"type\":\"keyword\"}," +
                "\"uprotocol\":{\"type\":\"keyword\"}," +
                "\"desc\":{\"type\":\"keyword\"}" +
                "}" +
                "}," +
                "\"srcIPLocation\":{" +
                "\"properties\":{" +
                "\"location\":{\"type\":\"keyword\"}," +
                "\"country\":{\"type\":\"keyword\"}," +
                "\"city\":{\"type\":\"keyword\"}," +
                "\"longitude\":{\"type\":\"double\"}," +
                "\"latitude\":{\"type\":\"double\"}" +
                "}" +
                "}," +
                "\"dstIPLocation\":{" +
                "\"properties\":{" +
                "\"location\":{\"type\":\"keyword\"}," +
                "\"country\":{\"type\":\"keyword\"}," +
                "\"city\":{\"type\":\"keyword\"}," +
                "\"longitude\":{\"type\":\"double\"}," +
                "\"latitude\":{\"type\":\"double\"}" +
                "}" +
                "}" +
                "}" +
                "}";

        return mapping;
    }

    @Override
    public String getIndexNamePrefix() {
        return "log_stream_session_tcp";
    }

    @Override
    public String getIndexDocType() {
        return Constants.ESLOGDOCTYPE;
    }

    @Override
    public String getSrcIP() {
        return IPUtils.ipv4Str(getReqIP());
    }

    @Override
    public String getDstIP() {
        return IPUtils.ipv4Str(getResIP());
    }

    private void appendParserData(XContentBuilder cb) throws IOException {

        XContentBuilder cbb = cb.startObject("parseData");

        StreamParserData  parserData = StreamParserPool.parse(this);

        if(parserData!=null){
            cbb.field("hasData",1);
            parserData.toJson(cbb);
        }else{

            cbb.field("hasData",0);
        }

        cbb.endObject();
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

    @Override
    public void read(DataInput in) throws IOException {

    }

    @Override
    public void parse(MessageUnpacker unpacker) throws IOException {

    }

}
