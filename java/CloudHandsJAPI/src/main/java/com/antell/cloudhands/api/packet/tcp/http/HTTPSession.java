package com.antell.cloudhands.api.packet.tcp.http;

import com.antell.cloudhands.api.packet.SessionEntry;
import com.antell.cloudhands.api.packet.security.MatchInfo;
import com.antell.cloudhands.api.packet.security.SecMatchResult;
import com.antell.cloudhands.api.packet.tcp.FileTranSession;
import com.antell.cloudhands.api.packet.tcp.TCPSessionEntry;
import com.antell.cloudhands.api.source.AbstractSourceEntry;
import com.antell.cloudhands.api.source.SourceEntry;
import com.antell.cloudhands.api.utils.*;
import com.google.common.base.Preconditions;
import org.elasticsearch.common.xcontent.XContentBuilder;
import org.msgpack.core.MessageUnpacker;

import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.List;

/**
 * Created by dell on 2018/6/11.
 */
public class HTTPSession extends AbstractSourceEntry {


    private SessionEntry sessionEntry;
    private String method;
    private String uri;
    private String extName;
    private String version;

    private String host;
    private String userAgent;
    private String server;

    private String  referer;
    private String reqContentType;
    private String resContentType;
    private String contentEncoding;
    private String contentDisposition;

    private int status;

    private String reqBodyPath;
    private String resBodyPath;

    private final List<Header> reqHeaders;
    private final List<Header> resHeaders;

    private SecMatchResult secMatchResult;

    /*only used for 206 part content parser*/
    private long lastTime;
    private String contentRange;
    private String filename;

    public HTTPSession(MessageUnpacker unpacker) throws IOException {

        sessionEntry = new TCPSessionEntry();
        reqHeaders = new ArrayList<>();
        resHeaders = new ArrayList<>();
        secMatchResult = null;
        host = null;
        referer = null;
        reqContentType = "";
        resContentType = "";
        contentEncoding = "";
        contentRange = "";
        contentDisposition = "";
        userAgent = "";
        server = "";
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
        fileTranSession.setParentObjectId(getObjectId());
        fileTranSession.setSrcIP(IPUtils.ipv4Str(sessionEntry.getReqIP()));
        fileTranSession.setSrcPort(sessionEntry.getReqPort());
        fileTranSession.setDstIP(IPUtils.ipv4Str(sessionEntry.getResIP()));
        fileTranSession.setDstPort(sessionEntry.getResPort());
        fileTranSession.setTime(sessionEntry.getReqStartTime());
        fileTranSession.setContentPath(path);

        fileTranSession.setFname(filename);
        fileTranSession.setExtName(extName);

        fileTranSession.setBytes(fsize);

        return fileTranSession;
    }


    private boolean canGenerteEvent(){

        if(!hasMatchSec())
            return false;

        MatchInfo matchInfo = getSecMatchResult().getMainMatchInfo();
        String type = matchInfo.getRuleType();

        return type.equalsIgnoreCase("BACKDOOR")||type.equalsIgnoreCase("WEB_ATTACK/WEBSHELL");
    }

    public boolean isPartContent(){

        return status == 206&&!TextUtils.isEmpty(contentRange);
    }

    public boolean canGenerateFTrans(){

        if(TextUtils.isEmpty(resBodyPath))
            return false;

        if(!TextUtils.isEmpty(contentDisposition)&&contentDisposition.contains("filename"))
            return true;

        if(isPartContent())
            return true;

        if(Content.getFsize(resBodyPath)>=1024*1024)
            return true;

       return  HttpFileTransTypes.isFileTrans(extName);
    }

    @Override
    public List<SourceEntry> generate(){

        List<SourceEntry> results = new ArrayList<>();

        if(!canGenerateFTrans()&&!canGenerteEvent())
            return null;

        if(canGenerateFTrans()) {
            long fsize = 0;

            try {
                fsize = Files.size(Paths.get(resBodyPath));
                results.add(toFileTranSession(resBodyPath,fsize));
            } catch (IOException e) {
                //return null;
            }
        }

        return results;
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
            if(isReq&&TextUtils.isEmpty(userAgent)){

                if(isHeader(k,"User-Agent"))
                    setUserAgent(v);
            }

            if(isReq&&TextUtils.isEmpty(reqContentType)){

                if(isHeader(k,"Content-Type"))
                    setReqContentType(v);
            }
            if(!isReq&&TextUtils.isEmpty(server)){

                if(isHeader(k,"Server"))
                    setServer(v);
            }

            if(!isReq&& TextUtils.isEmpty(resContentType)){
                if(isHeader(k,"Content-Type"))
                    setResContentType(v);
            }

            if(!isReq&& TextUtils.isEmpty(contentEncoding)){
                if(isHeader(k,"Content-Encoding"))
                    setContentEncoding(v);
            }

            if(!isReq&&TextUtils.isEmpty(contentRange)){
                if(isHeader(k,"Content-Range"))
                    setContentRange(v);
            }
            if(!isReq&&TextUtils.isEmpty(contentDisposition)){
                if(isHeader(k,"Content-Disposition"))
                    setContentDisposition(v);
            }
        }

    }

    public void parse(MessageUnpacker unpacker) throws IOException {

        long reqBSize = 0;
        long resBSize = 0;

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

        reqBSize = Content.getFsize(reqBodyPath);
        resBSize = Content.getFsize(resBodyPath);
        if(reqBSize>0)
            ((TCPSessionEntry)sessionEntry).setReqPBytes(reqBSize);

        if(resBSize>0)
            ((TCPSessionEntry)sessionEntry).setResPBytes(resBSize);

        unpackHeaders(unpacker,true);
        unpackHeaders(unpacker,false);

        if(hasSecMatch){

            /*parse attack check info*/
            secMatchResult = new SecMatchResult(unpacker);

        }

        if(TextUtils.isEmpty(host))
            host = IPUtils.ipv4Str(sessionEntry.getResIP());

        setFileInfo();
    }

    private void setFileInfo(){

        String fname = null;
        if(!TextUtils.isEmpty(contentDisposition)){
            fname = Content.getFileName(contentDisposition);
        }

        if(TextUtils.isEmpty(fname)){

            fname = Content.getFileName(resContentType,uri,resBodyPath);
        }
        if(TextUtils.isEmpty(fname)){
            setFilename("unknown_file");
            setExtName("data");
        }else{

            setFilename(fname);
            setExtName(Content.getExtName(fname));
        }
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

        cb.field("objectId",getObjectId());
        cb.field("method",TextUtils.getStrValue(method));
        cb.field("uri",TextUtils.getStrValue(uri));
        cb.field("extName",TextUtils.getStrValue(extName));
        cb.field("host",TextUtils.getStrValue(host));
        cb.field("userAgent",userAgent);
        cb.field("server",server);

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

        cb.field("sdh",sessionEntry.getReqIP()+"|"+sessionEntry.getResIP()+"|"+host);


        if(hasMatchSec()){
            XContentBuilder attkCB = cb.startObject("attack");
            secMatchResult.dataToJson(attkCB);
            attkCB.endObject();
        }

        return cb;
    }

    @Override
    public String getIndexMapping() {
        String mapping = "";
        if(!hasMatchSec()){
        mapping = "{" +
                "\"properties\":{" +
                "\"id\":{\"type\":\"keyword\"}," +
                "\"objectId\":{\"type\":\"keyword\"}," +
                "\"sessionEntry\":{" +
                "\"properties\":{" +
                "\"sessionID\":{\"type\":\"long\"}," +
                "\"protocol\":{\"type\":\"keyword\"}," +
                "\"srcIP\":{\"type\":\"keyword\"}," +
                "\"dstIP\":{\"type\":\"keyword\"}," +
                "\"srcPort\":{\"type\":\"integer\"}," +
                "\"dstPort\":{\"type\":\"integer\"}," +
                "\"reqStartTime\":{\"type\":\"long\"}," +
                "\"resStartTime\":{\"type\":\"long\"}," +
                "\"timeDate\":{\"type\":\"date\",\"format\":\"yyyy-MM-dd HH:mm:ss\"}," +
                "\"reqPackets\":{\"type\":\"long\"}," +
                "\"reqBytes\":{\"type\":\"long\"}," +
                "\"reqPBytes\":{\"type\":\"long\"}," +
                "\"resPackets\":{\"type\":\"long\"}," +
                "\"resBytes\":{\"type\":\"long\"}," +
                "\"resPBytes\":{\"type\":\"long\"}" +
                "}" +
                "}," +
                "\"method\":{\"type\":\"keyword\"}," +
                "\"uri\":{\"type\":\"keyword\"}," +
                "\"extName\":{\"type\":\"keyword\"}," +
                "\"host\":{\"type\":\"keyword\"}," +
                "\"userAgent\":{\"type\":\"keyword\"}," +
                "\"server\":{\"type\":\"keyword\"}," +
                "\"sdh\":{\"type\":\"keyword\"}," +
                "\"referer\":{\"type\":\"keyword\"}," +
                "\"reqContentType\":{\"type\":\"keyword\"}," +
                "\"resContentType\":{\"type\":\"keyword\"}," +
                "\"contentEncoding\":{\"type\":\"keyword\"}," +
                "\"version\":{\"type\":\"keyword\"}," +
                "\"status\":{\"type\":\"integer\"}," +
                "\"reqBodyPath\":{\"type\":\"keyword\"}," +
                "\"resBodyPath\":{\"type\":\"keyword\"}," +
                "\"reqHeaders\":{\"type\":\"keyword\"}," +
                "\"resHeaders\":{\"type\":\"keyword\"}," +
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
        }else {
            mapping = "{" +
                    "\"properties\":{" +
                    "\"id\":{\"type\":\"keyword\"}," +
                    "\"objectId\":{\"type\":\"keyword\"}," +
                    "\"sessionEntry\":{" +
                    "\"properties\":{" +
                    "\"sessionID\":{\"type\":\"long\"}," +
                    "\"protocol\":{\"type\":\"keyword\"}," +
                    "\"srcIP\":{\"type\":\"keyword\"}," +
                    "\"dstIP\":{\"type\":\"keyword\"}," +
                    "\"srcPort\":{\"type\":\"integer\"}," +
                    "\"dstPort\":{\"type\":\"integer\"}," +
                    "\"reqStartTime\":{\"type\":\"long\"}," +
                    "\"resStartTime\":{\"type\":\"long\"}," +
                    "\"timeDate\":{\"type\":\"date\",\"format\":\"yyyy-MM-dd HH:mm:ss\"}," +
                    "\"reqPackets\":{\"type\":\"long\"}," +
                    "\"reqBytes\":{\"type\":\"long\"}," +
                    "\"reqPBytes\":{\"type\":\"long\"}," +
                    "\"resPackets\":{\"type\":\"long\"}," +
                    "\"resBytes\":{\"type\":\"long\"}," +
                    "\"resPBytes\":{\"type\":\"long\"}" +
                    "}" +
                    "}," +
                    "\"method\":{\"type\":\"keyword\"}," +
                    "\"uri\":{\"type\":\"keyword\"}," +
                    "\"extName\":{\"type\":\"keyword\"}," +
                    "\"host\":{\"type\":\"keyword\"}," +
                    "\"userAgent\":{\"type\":\"keyword\"}," +
                    "\"server\":{\"type\":\"keyword\"}," +
                    "\"sdh\":{\"type\":\"keyword\"}," +
                    "\"referer\":{\"type\":\"keyword\"}," +
                    "\"reqContentType\":{\"type\":\"keyword\"}," +
                    "\"resContentType\":{\"type\":\"keyword\"}," +
                    "\"contentEncoding\":{\"type\":\"keyword\"}," +
                    "\"version\":{\"type\":\"keyword\"}," +
                    "\"status\":{\"type\":\"integer\"}," +
                    "\"reqBodyPath\":{\"type\":\"keyword\"}," +
                    "\"resBodyPath\":{\"type\":\"keyword\"}," +
                    "\"reqHeaders\":{\"type\":\"keyword\"}," +
                    "\"resHeaders\":{\"type\":\"keyword\"}," +
                    "\"attack\":{" +
                    "\"properties\":{" +
                    "\"mainEngineName\":{\"type\":\"keyword\"}," +
                    "\"mainEngineType\":{\"type\":\"integer\"}," +
                    "\"mainEngineLevel\":{\"type\":\"integer\"}," +
                    "\"mainRuleID\":{\"type\":\"long\"}," +
                    "\"mainRuleLevel\":{\"type\":\"long\"}," +
                    "\"mainRuleMsg\":{\"type\":\"keyword\"}," +
                    "\"mainRulePayload\":{\"type\":\"keyword\"}," +
                    "\"mainRuleType\":{\"type\":\"keyword\"}," +
                    "\"mainMatchVarName\":{\"type\":\"keyword\"}," +
                    "\"mainMatchVarValue\":{\"type\":\"keyword\"}," +
                    "\"matchInfoList\":{\"type\":\"nested\"}" +
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
        }

        return mapping;
    }

    @Override
    public String getIndexNamePrefix() {
        if(hasMatchSec())
            return "alert_tcp_session_http";

        return "log_tcp_session_http";
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

    public String getReferer() {
        return referer;
    }

    public void setReferer(String referer) {
        this.referer = referer;
    }

    @Override
    public int hashCode() {
        return get206Key().hashCode();
    }

    @Override
    public boolean equals(Object obj) {

        HTTPSession s = (HTTPSession)obj;

        if(s.getSessionEntry().getReqIP()!=sessionEntry.getReqIP())
            return false;

        if(!s.getHost().equalsIgnoreCase(host))
            return false;

        return s.getUri().equals(uri);
    }

    public String get206Key(){
        StringBuffer sb = new StringBuffer();
        sb.append(sessionEntry.getReqIP());
        sb.append(host);
        sb.append(uri);

        return sb.toString();
    }

    public long getLastTime() {
        return lastTime;
    }

    public void setLastTime(long lastTime) {
        this.lastTime = lastTime;
    }

    public String getContentRange() {
        return contentRange;
    }

    public void setContentRange(String contentRange) {
        this.contentRange = contentRange;
    }

    public String getContentDisposition() {
        return contentDisposition;
    }

    public void setContentDisposition(String contentDisposition) {
        this.contentDisposition = contentDisposition;
    }


    public String getFilename() {
        return filename;
    }

    public void setFilename(String filename) {
        this.filename = filename;
    }

    public String getProto(){

        return "http";
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

    public String getUserAgent() {
        return userAgent;
    }

    public void setUserAgent(String userAgent) {
        this.userAgent = userAgent;
    }

    public String getServer() {
        return server;
    }

    public void setServer(String server) {
        this.server = server;
    }
}
