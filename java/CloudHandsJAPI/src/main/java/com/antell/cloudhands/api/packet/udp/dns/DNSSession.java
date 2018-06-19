package com.antell.cloudhands.api.packet.udp.dns;

import com.antell.cloudhands.api.BinDataInput;
import com.antell.cloudhands.api.source.SourceEntry;
import org.elasticsearch.common.xcontent.XContentBuilder;

import java.io.DataInput;
import java.io.IOException;

/**
 * Created by dell on 2018/6/11.
 */
public class DNSSession  implements SourceEntry {

    private DNSSessionEntry sessionEntry;
    private DNSRequst dnsRequst;
    private DNSResponse dnsResponse;

    public DNSSession(DataInput in) throws IOException {
        sessionEntry = new DNSSessionEntry();
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
    public XContentBuilder dataToJson(XContentBuilder cb) throws IOException {

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

    public DNSSessionEntry getSessionEntry() {
        return sessionEntry;
    }
}
