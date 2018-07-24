package com.antell.cloudhands.api.packet.tcp.http;

import com.antell.cloudhands.api.packet.SessionEntry;
import com.antell.cloudhands.api.packet.security.SecMatchResult;
import com.antell.cloudhands.api.packet.tcp.FileTranSession;
import com.antell.cloudhands.api.packet.tcp.TCPSessionEntry;
import com.antell.cloudhands.api.source.SourceEntry;
import com.antell.cloudhands.api.utils.*;
import com.google.common.base.Preconditions;
import org.elasticsearch.common.xcontent.XContentBuilder;
import org.msgpack.core.MessageUnpacker;

import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.*;

/**
 * Created by dell on 2018/6/11.
 */
public class HTTPSession implements SourceEntry{

    private final String objectId;
    private SessionEntry sessionEntry;
    private String method;
    private String uri;
    private String extName;
    private String version;

    private String host;
    private String  referer;
    private String reqContentType;
    private String resContentType;
    private String contentEncoding;

    private int status;

    private String reqBodyPath;
    private String resBodyPath;

    private final List<Header> reqHeaders;
    private final List<Header> resHeaders;

    private SecMatchResult secMatchResult;



    public HTTPSession(MessageUnpacker unpacker) throws IOException {

        objectId = TextUtils.getUUID();
        sessionEntry = new TCPSessionEntry();
        reqHeaders = new ArrayList<>();
        resHeaders = new ArrayList<>();
        secMatchResult = null;
        host = null;
        referer = null;
        reqContentType = "";
        resContentType = "";
        contentEncoding = "";

        parse(unpacker);
    }

    private final static String getFileName(String path){

        int index = path.lastIndexOf("/");
        if(index<0)
            return path;

        return path.substring(index+1);

    }
    private   FileTranSession toFileTranSession(String path,long fsize){

        FileTranSession fileTranSession = new FileTranSession();

        fileTranSession.setProto("http");
        fileTranSession.setParentObjectId(objectId);
        fileTranSession.setSrcIP(IPUtils.ipv4Str(sessionEntry.getReqIP()));
        fileTranSession.setSrcPort(sessionEntry.getReqPort());
        fileTranSession.setDstIP(IPUtils.ipv4Str(sessionEntry.getResIP()));
        fileTranSession.setDstPort(sessionEntry.getResPort());
        fileTranSession.setTime(sessionEntry.getReqStartTime());
        fileTranSession.setContentPath(path);
        fileTranSession.setFname(getFileName(path));
        fileTranSession.setExtName(extName);
        fileTranSession.setBytes(fsize);

        return fileTranSession;
    }

    @Override
    public List<SourceEntry> generate(){

        if(TextUtils.isEmpty(resBodyPath)||!HttpFileTransTypes.isFileTrans(extName))
            return null;

        long fsize = 0;

        try {
            fsize =  Files.size(Paths.get(resBodyPath));
        } catch (IOException e) {
            return  null;
        }


        return Arrays.asList(toFileTranSession(resBodyPath,fsize));

    }

    private final  static boolean isHeader(String k,String sk){

        return k.equalsIgnoreCase(sk);
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

            if(isReq&&TextUtils.isEmpty(host)){
                if(isHeader(k,"Host"))
                    setHost(v);
            }
            if(isReq&&TextUtils.isEmpty(referer)){
                if(isHeader(k,"Referer"))
                    setReferer(v);
            }

            if(isReq&&TextUtils.isEmpty(reqContentType)){

                if(isHeader(k,"Content-Type"))
                    setReqContentType(v);
            }
            if(!isReq&& TextUtils.isEmpty(resContentType)){
                if(isHeader(k,"Content-Type"))
                    setResContentType(v);
            }

            if(!isReq&& TextUtils.isEmpty(contentEncoding)){
                if(isHeader(k,"Content-Encoding"))
                    setContentEncoding(v);
            }


        }

    }


    public void parse(MessageUnpacker unpacker) throws IOException {

        int n = MessagePackUtil.parseMapHeader(unpacker,false);
        Preconditions.checkArgument(n==2||n==3,"Invalid http session messagePack:"+n);

        boolean hasSecMatch = n==3;

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

        if(hasSecMatch){

            /*parse attack check info*/
            secMatchResult = new SecMatchResult(unpacker);

        }

        setExtName(Content.getExtName(resContentType,uri));

    }

    private boolean hasMatchSec(){

        if(secMatchResult == null)
            return false;

        return secMatchResult.getMatchCount()>0;
    }

    @Override
    public String dataToString() {

        StringBuffer sb = new StringBuffer();

        sb.append(sessionEntry.dataToString());
        TextUtils.addText(sb,"method",method);
        TextUtils.addText(sb,"uri",uri);
        TextUtils.addText(sb,"extName",extName);
        TextUtils.addText(sb,"host",host);
        TextUtils.addText(sb,"referer",referer);
        TextUtils.addText(sb,"reqContentType",reqContentType);
        TextUtils.addText(sb,"resContentType",resContentType);
        TextUtils.addText(sb,"contentEncoding",contentEncoding);
        TextUtils.addText(sb,"version",version);
        TextUtils.addInt(sb,"status",status);
        TextUtils.addText(sb,"reqBodyPath",reqBodyPath);
        TextUtils.addText(sb,"resBodyPath",resBodyPath);
        TextUtils.addList(sb,"reqHeaders",reqHeaders);
        TextUtils.addList(sb,"resHeaders",resHeaders);

        if(hasMatchSec()){

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

        cb.field("method",TextUtils.getStrValue(method));
        cb.field("uri",TextUtils.getStrValue(uri));
        cb.field("extName",TextUtils.getStrValue(extName));
        cb.field("host",TextUtils.getStrValue(host));
        cb.field("referer",TextUtils.getStrValue(referer));
        cb.field("reqContentType",TextUtils.getStrValue(reqContentType));
        cb.field("resContentType",TextUtils.getStrValue(resContentType));
        cb.field("contentEncoding",TextUtils.getStrValue(contentEncoding));
        cb.field("version",TextUtils.getStrValue(version));
        cb.field("status",status);
        cb.field("reqBodyPath",TextUtils.getStrValue(reqBodyPath));
        cb.field("resBodyPath",TextUtils.getStrValue(resBodyPath));
        cb.field("reqHeaders",reqHeaders);
        cb.field("resHeaders",resHeaders);

        if(hasMatchSec()){
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

    public SessionEntry getSessionEntry() {
        return sessionEntry;
    }


    public String getExtName() {
        return extName;
    }

    public void setExtName(String extName) {
        this.extName = extName;
    }

    public String getReqContentType() {
        return reqContentType;
    }

    public void setReqContentType(String reqContentType) {
        this.reqContentType = reqContentType;
    }

    public String getResContentType() {
        return resContentType;
    }

    public void setResContentType(String resContentType) {
        this.resContentType = resContentType;
    }

    public String getContentEncoding() {
        return contentEncoding;
    }

    public void setContentEncoding(String contentEncoding) {
        this.contentEncoding = contentEncoding;
    }

    public String getObjectId() {
        return objectId;
    }

    public String getReferer() {
        return referer;
    }

    public void setReferer(String referer) {
        this.referer = referer;
    }
}
