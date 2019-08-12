package com.antell.cloudhands.api.packet;

import com.antell.cloudhands.api.source.SourceEntry;
import com.antell.cloudhands.api.utils.Constants;
import com.antell.cloudhands.api.utils.DateUtils;
import com.antell.cloudhands.api.utils.IPUtils;
import com.antell.cloudhands.api.utils.TextUtils;
import org.elasticsearch.common.xcontent.XContentBuilder;
import org.msgpack.core.MessageUnpacker;

import java.io.DataInput;
import java.io.IOException;

/**
 * Created by dell on 2018/4/16.
 */
public class UDPSession extends SessionEntry implements SourceEntry{

    private final String objectId;


    public  UDPSession(DataInput input) throws IOException {

        objectId = TextUtils.getUUID();
        setTimeout(input.readUnsignedByte()!=0);
        setTimeoutTV(input.readUnsignedShort());
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
    public XContentBuilder dataToJson(XContentBuilder cb) throws IOException {

        cb.field("proto","udp");
        cb.field("isIPV6",isIPV6()?1:0);
        cb.field("srcIP", getsrcIPStr());
        cb.field("dstIP",getdstIPStr());
        cb.field("srcPort",getReqPort());
        cb.field("dstPort",getResPort());
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

        PortItem portItem = Portmap.getPortItem(getReqPort(),getResPort());
        portItem.toJson(cbb);

        cbb.endObject();

        return cb;
    }

    @Override
    public String getIndexMapping() {

        String mapping = "{" +
                "\"properties\":{" +
                "\"id\":{\"type\":\"keyword\"}," +
                "\"proto\":{\"type\":\"keyword\"}," +
                "\"srcIP\":{\"type\":\"keyword\"}," +
                "\"dstIP\":{\"type\":\"keyword\"}," +
                "\"portScan\":{\"type\":\"keyword\"}," +
                "\"sdp\":{\"type\":\"keyword\"}," +
                "\"srcPort\":{\"type\":\"integer\"}," +
                "\"dstPort\":{\"type\":\"integer\"}," +
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
        return "log_stream_session_udp";
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

    @Override
    public String dataToString() {
        StringBuffer sb = new StringBuffer();
        sb.append("Dump UDP Packet Session informations:\n");
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

    @Override
    public String getObjectId() {
        return objectId;
    }
}
