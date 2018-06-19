package com.antell.cloudhands.api.packet.tcp.http;

import com.antell.cloudhands.api.packet.security.SecMatchResult;
import com.antell.cloudhands.api.packet.tcp.TCPSessionEntry;
import com.antell.cloudhands.api.source.SourceEntry;
import com.antell.cloudhands.api.utils.MessagePackUtil;
import com.antell.cloudhands.api.utils.TextUtils;
import com.google.common.base.Preconditions;
import org.elasticsearch.common.xcontent.XContentBuilder;
import org.msgpack.core.MessageUnpacker;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

/**
 * Created by dell on 2018/6/11.
 */
public class HTTPSession implements SourceEntry{

    private TCPSessionEntry sessionEntry;
    private String method;
    private String uri;
    private String version;
    private String host;

    private int status;

    private String reqBodyPath;
    private String resBodyPath;

    private final List<Header> reqHeaders;
    private final List<Header> resHeaders;

    private SecMatchResult secMatchResult;

    public HTTPSession(MessageUnpacker unpacker) throws IOException {

        sessionEntry = new TCPSessionEntry();
        reqHeaders = new ArrayList<>();
        resHeaders = new ArrayList<>();
        secMatchResult = null;
        host = null;

        parse(unpacker);
    }

    private boolean isHost(String k){

        if(k.length()!=4)
            return false;

        return k.equalsIgnoreCase("Host");
    }

    private void unpackHeaders(MessageUnpacker unpacker,boolean isReq) throws IOException {

        List<Header> headers = isReq?reqHeaders:resHeaders;

        unpacker.unpackString();
        int n = unpacker.unpackMapHeader();
        String k;
        String v;

        for(int i = 0; i<n;i++){

            k = unpacker.unpackString();
            v = unpacker.unpackString();

            Header header = new Header(k,v);
            headers.add(header);

            if(!isReq||host!=null)
                continue;

            if(isHost(k))
                setHost(v);
        }

    }


    public void parse(MessageUnpacker unpacker) throws IOException {

        int n = MessagePackUtil.parseMapHeader(unpacker,false);

        Preconditions.checkArgument(n==2||n==3,"Invalid http session messagePack:"+n);

        /*parse session entry */
        sessionEntry.parse(unpacker);

        /*skip http proto name and to parse http proto info*/
        n = MessagePackUtil.parseMapHeader(unpacker,true);
        Preconditions.checkArgument(n == 8,"Invalid http session msgpack packet:"+n);

        setMethod(MessagePackUtil.parseText(unpacker));
        setUri(MessagePackUtil.parseText(unpacker));
        setVersion(MessagePackUtil.parseText(unpacker));
        setStatus(MessagePackUtil.parseInt(unpacker));
        setReqBodyPath(MessagePackUtil.parseText(unpacker));
        setResBodyPath(MessagePackUtil.parseText(unpacker));

        unpackHeaders(unpacker,true);
        unpackHeaders(unpacker,false);

        if(n == 3){

            /*parse attack check info*/
            secMatchResult = new SecMatchResult();

            secMatchResult.parse(unpacker);

        }
    }

    @Override
    public String dataToString() {

        StringBuffer sb = new StringBuffer();

        sb.append(sessionEntry.dataToString());
        TextUtils.addText(sb,"method",method);
        TextUtils.addText(sb,"uri",uri);
        TextUtils.addText(sb,"host",host);
        TextUtils.addText(sb,"version",version);
        TextUtils.addInt(sb,"status",status);
        TextUtils.addText(sb,"reqBodyPath",reqBodyPath);
        TextUtils.addText(sb,"resBodyPath",resBodyPath);
        TextUtils.addList(sb,"reqHeaders",reqHeaders);
        TextUtils.addList(sb,"resHeaders",resHeaders);

        if(secMatchResult!=null){

            sb.append(secMatchResult.dataToString());
        }

        return sb.toString();

    }

    public String toString(){
        return dataToString();
    }

    @Override
    public XContentBuilder dataToJson(XContentBuilder cb) throws IOException {

        XContentBuilder seCB = cb.startObject("sessionEntry");
        sessionEntry.dataToJson(seCB);
        seCB.endObject();

        cb.field("method",method);
        cb.field("uri",uri);
        cb.field("host",host);
        cb.field("version",version);
        cb.field("status",status);
        cb.field("reqBodyPath",reqBodyPath);
        cb.field("resBodyPath",resBodyPath);
        cb.field("reqHeaders",reqHeaders);
        cb.field("resHeaders",resHeaders);

        if(secMatchResult!=null){
            XContentBuilder attkCB = cb.startObject("attack");
            secMatchResult.dataToJson(attkCB);
            attkCB.endObject();
        }

        return cb;
    }

    public List<Header> getReqHeaders() {
        return reqHeaders;
    }

    public List<Header> getResHeaders() {
        return resHeaders;
    }

    public String getMethod() {
        return method;
    }

    public void setMethod(String method) {
        this.method = method;
    }

    public String getUri() {
        return uri;
    }

    public void setUri(String uri) {
        this.uri = uri;
    }

    public String getVersion() {
        return version;
    }

    public void setVersion(String version) {
        this.version = version;
    }

    public String getHost() {
        return host;
    }

    public void setHost(String host) {
        this.host = host;
    }

    public int getStatus() {
        return status;
    }

    public void setStatus(int status) {
        this.status = status;
    }

    public String getReqBodyPath() {
        return reqBodyPath;
    }

    public void setReqBodyPath(String reqBodyPath) {
        this.reqBodyPath = reqBodyPath;
    }

    public String getResBodyPath() {
        return resBodyPath;
    }

    public void setResBodyPath(String resBodyPath) {
        this.resBodyPath = resBodyPath;
    }

    public SecMatchResult getSecMatchResult() {
        return secMatchResult;
    }

    public void setSecMatchResult(SecMatchResult secMatchResult) {
        this.secMatchResult = secMatchResult;
    }

    public TCPSessionEntry getSessionEntry() {
        return sessionEntry;
    }


}
