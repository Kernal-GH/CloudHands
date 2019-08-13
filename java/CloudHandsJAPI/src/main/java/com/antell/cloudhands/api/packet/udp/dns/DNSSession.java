package com.antell.cloudhands.api.packet.udp.dns;

import com.antell.cloudhands.api.packet.SessionEntry;
import com.antell.cloudhands.api.packet.udp.UDPSessionEntry;
import com.antell.cloudhands.api.source.AbstractSourceEntry;
import com.antell.cloudhands.api.utils.Constants;
import com.antell.cloudhands.api.utils.IPUtils;
import com.antell.cloudhands.api.utils.MessagePackUtil;
import com.antell.cloudhands.api.utils.TextUtils;
import com.google.common.base.Preconditions;
import org.elasticsearch.common.xcontent.XContentBuilder;
import org.msgpack.core.MessageUnpacker;

import java.io.DataInput;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

/**
 * Created by dell on 2018/6/11.
 */
public class DNSSession  extends AbstractSourceEntry {


    private SessionEntry sessionEntry;
    private DNSRequst dnsRequst;
    private DNSResponse dnsResponse;

    public DNSSession(DataInput in) throws IOException {

        sessionEntry = new UDPSessionEntry();
        dnsRequst = null;
        dnsResponse = null;
        sessionEntry.read(in);
        sessionEntry.setProtocol("DNS");
        boolean hasReq = in.readUnsignedByte()==1;
        boolean hasRes = in.readUnsignedByte()==1;

        if(hasReq){

            dnsRequst = new DNSRequst();
            dnsRequst.read(in);
        }

        if(hasRes){

            dnsResponse = new DNSResponse();
            dnsResponse.read(in);
        }

    }

    public DNSSession(MessageUnpacker unpacker) throws IOException {

        sessionEntry = new UDPSessionEntry();
        dnsRequst = null;
        dnsResponse = null;

        int n = MessagePackUtil.parseMapHeader(unpacker,false);
        Preconditions.checkArgument(n==2,"Invalid msgpack packet of dns session entry:"+n);

        sessionEntry.parse(unpacker);

        n = MessagePackUtil.parseMapHeader(unpacker,true);
        Preconditions.checkArgument(n==3||n==4,"Invalid msgpack packet of dns session entry:"+n);
        boolean hasReq = MessagePackUtil.parseByte(unpacker)==1;
        boolean hasRes = MessagePackUtil.parseByte(unpacker)==1;

        if(hasReq){

            dnsRequst = new DNSRequst();
            dnsRequst.parse(unpacker);
        }

        if(hasRes){

            dnsResponse = new DNSResponse();
            dnsResponse.parse(unpacker);
        }
    }

    @Override
    public String getProto(){

        return "dns";
    }

    public long getSrcIPI(){

        return sessionEntry.getReqIP();
    }

    public long getDstIPI(){
        return sessionEntry.getResIP();
    }

    public long getTime(){

        return sessionEntry.getReqStartTime();
    }


    private  static final String[] getSubDomains(String domain){

        String[] res = new String[2];

        res[0] = "";
        res[1] = "";

        if(!TextUtils.isEmpty(domain)) {
            String[] strings = domain.split("\\.");

            if(strings.length<=2){
                res[0]=domain;
                res[1]=domain;
            }else{

                res[0] = String.format("%s.%s",strings[strings.length-2],strings[strings.length-1]);
                res[1] = domain.substring(0,domain.length()-res[0].length()-1);
            }
        }

        return res;

    }

    @Override
    public XContentBuilder dataToJson(XContentBuilder cb) throws IOException {

        cb.field("objectId",getObjectId());

        String domain = "";
        if(dnsRequst!=null)
            domain = dnsRequst.getDomain();

        cb.field("domain",domain);
        cb.field("domainLen",domain.length());

        String[] subs = getSubDomains(domain);
        cb.field("rootDomain",subs[0]);
        cb.field("subDomain",subs[1]);
        cb.field("ipRootDomain",sessionEntry.getReqIP()+"|"+subs[0]);

        List<String> ipList = dnsResponse==null?new ArrayList<>():dnsResponse.getIPV4Adresses();

        cb.field("ipCount",ipList.size());
        cb.field("ipList",ipList);

        XContentBuilder seCB = cb.startObject("sessionEntry");
        sessionEntry.dataToJson(seCB);
        seCB.endObject();

        XContentBuilder reqCB = cb.startObject("request");
        if(dnsRequst!=null)
            dnsRequst.dataToJson(reqCB);
        reqCB.endObject();

        XContentBuilder resCB = cb.startObject("response");
        if(dnsResponse!=null)
            dnsResponse.dataToJson(resCB);
        resCB.endObject();

        return cb;
    }

    @Override
    public String getIndexMapping() {
        String mapping = "{" +
                "\"properties\":{" +
                "\"id\":{\"type\":\"keyword\"}," +
                "\"objectId\":{\"type\":\"keyword\"}," +
                "\"domain\":{\"type\":\"keyword\"}," +
                "\"rootDomain\":{\"type\":\"keyword\"}," +
                "\"subDomain\":{\"type\":\"keyword\"}," +
                "\"ipRootDomain\":{\"type\":\"keyword\"}," +
                "\"domainLen\":{\"type\":\"integer\"}," +
                "\"ipList\":{\"type\":\"keyword\"}," +
                "\"ipCount\":{\"type\":\"integer\"}," +
                "\"sessionEntry\":{" +
                "\"properties\":{" +
                "\"sessionID\":{\"type\":\"long\"}," +
                "\"srcIP\":{\"type\":\"keyword\"}," +
                "\"dstIP\":{\"type\":\"keyword\"}," +
                "\"srcPort\":{\"type\":\"integer\"}," +
                "\"dstPort\":{\"type\":\"integer\"}," +
                "\"reqPackets\":{\"type\":\"long\"}," +
                "\"reqBytes\":{\"type\":\"long\"}," +
                "\"resPackets\":{\"type\":\"long\"}," +
                "\"resBytes\":{\"type\":\"long\"}," +
                "\"reqStartTime\":{\"type\":\"long\"}," +
                "\"reqLastTime\":{\"type\":\"long\"}," +
                "\"resStartTime\":{\"type\":\"long\"}," +
                "\"resLastTime\":{\"type\":\"long\"}," +
                "\"timeDate\":{\"type\":\"date\",\"format\":\"yyyy-MM-dd HH:mm:ss\"}" +
                "}" +
                "}," +
                "\"request\":{" +
                "\"properties\":{" +
                "\"reqHeader\":{" +
                "\"properties\":{" +
                "\"opcode\":{\"type\":\"keyword\"}," +
                "\"status\":{\"type\":\"keyword\"}," +
                "\"id\":{\"type\":\"integer\"}," +
                "\"flags\":{\"type\":\"keyword\"}," +
                "\"qd\":{\"type\":\"integer\"}," +
                "\"an\":{\"type\":\"integer\"}," +
                "\"au\":{\"type\":\"integer\"}," +
                "\"ad\":{\"type\":\"integer\"}" +
                "}" +
                "}" +
                "}" +
                "}," +
                "\"response\":{" +
                "\"properties\":{" +
                "\"resHeader\":{" +
                "\"properties\":{" +
                "\"opcode\":{\"type\":\"keyword\"}," +
                "\"status\":{\"type\":\"keyword\"}," +
                "\"id\":{\"type\":\"integer\"}," +
                "\"flags\":{\"type\":\"keyword\"}," +
                "\"qd\":{\"type\":\"integer\"}," +
                "\"an\":{\"type\":\"integer\"}," +
                "\"au\":{\"type\":\"integer\"}," +
                "\"ad\":{\"type\":\"integer\"}" +
                "}" +
                "}" +
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
        return "log_udp_session_dns";
    }

    @Override
    public String getIndexDocType() {
        return Constants.ESLOGDOCTYPE;
    }

    @Override
    public String getSrcIP() {
        return IPUtils.ipv4Str(sessionEntry.getReqIP());
    }

    @Override
    public String getDstIP() {
        return IPUtils.ipv4Str(sessionEntry.getResIP());
    }

    @Override
    public String dataToString() {

        StringBuffer sb = new StringBuffer();
        sb.append(sessionEntry.dataToString());

        sb.append("DNS.Request.info::\n");
        if(dnsRequst!=null)
            sb.append(dnsRequst.dataToString());

        sb.append("DNS.Response.info::\n");
        if(dnsResponse!=null)
            sb.append(dnsResponse.dataToString());

        return sb.toString();
    }

    @Override
    public String toString(){

        return dataToString();
    }

    public DNSRequst getDnsRequst() {
        return dnsRequst;
    }

    public DNSResponse getDnsResponse() {
        return dnsResponse;
    }

    public SessionEntry getSessionEntry() {
        return sessionEntry;
    }


}
