package com.antell.cloudhands.api.packet.udp.dns;

import com.antell.cloudhands.api.packet.SessionEntry;
import com.antell.cloudhands.api.packet.security.AttackEvent;
import com.antell.cloudhands.api.packet.udp.UDPSessionEntry;
import com.antell.cloudhands.api.source.AbstractSourceEntry;
import com.antell.cloudhands.api.source.SourceEntry;
import com.antell.cloudhands.api.utils.TextUtils;
import org.elasticsearch.common.xcontent.XContentBuilder;

import java.io.DataInput;
import java.io.IOException;

/**
 * Created by dell on 2018/6/11.
 */
public class DNSSession  extends AbstractSourceEntry {

    private AttackEvent event;

    private SessionEntry sessionEntry;
    private DNSRequst dnsRequst;
    private DNSResponse dnsResponse;

    public DNSSession(DataInput in) throws IOException {

        event = null;
        sessionEntry = new UDPSessionEntry();
        dnsRequst = null;
        dnsResponse = null;
        sessionEntry.read(in);

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


    @Override
    public AttackEvent getEvent() {
        return event;
    }

    @Override
    public void setEvent(AttackEvent event) {
        this.event = event;
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

    @Override
    public XContentBuilder dataToJson(XContentBuilder cb) throws IOException {

        cb.field("objectId",getObjectId());
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
    public String dataToString() {

        StringBuffer sb = new StringBuffer();
        sb.append(sessionEntry.dataToString());

        sb.append("DNS.Request.info::\n\n");
        if(dnsRequst!=null)
            sb.append(dnsRequst.dataToString());

        sb.append("DNS.Response.info::\n\n");
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
