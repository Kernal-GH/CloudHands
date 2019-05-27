package com.antell.cloudhands.api.packet.tcp.mail;

import com.antell.cloudhands.api.packet.SessionEntry;
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
import java.util.*;
import java.util.stream.Collectors;

/**
 * Created by dell on 2018/6/11.
 */
public class MailSession extends AbstractSourceEntry {

    private SessionEntry sessionEntry;

    private String userName;
    private String passwd;
    private String from;
    private String subject;
    private String contentTxtPath;
    private String contentHtmlPath;
    private int attachNum;
    private int mailCCNum;
    private int mailToNum;

    private final List<MaillAttachEntry> maillAttachEntryList;
    private final List<String> mailCCList;
    private final List<String> mailToList;
    private SecMatchResult secMatchResult;

    public MailSession(MessageUnpacker unpacker) throws IOException {

        this.sessionEntry = new TCPSessionEntry();
        this.maillAttachEntryList = new ArrayList<>();
        this.mailCCList = new ArrayList<>();
        this.mailToList = new ArrayList<>();
        this.secMatchResult = null;

        parse(unpacker);
    }

    public String getUserName() {
        return userName;
    }

    public void setUserName(String userName) {
        this.userName = userName;
    }

    public String getPasswd() {
        return passwd;
    }

    public void setPasswd(String passwd) {
        this.passwd = passwd;
    }

    public String getFrom() {
        return from;
    }

    public void setFrom(String from) {
        this.from = from;
    }

    public String getSubject() {
        return subject;
    }

    public void setSubject(String subject) {
        this.subject = subject;
    }

    public String getContentTxtPath() {
        return contentTxtPath;
    }

    public void setContentTxtPath(String contentTxtPath) {
        this.contentTxtPath = contentTxtPath;
    }

    public String getContentHtmlPath() {
        return contentHtmlPath;
    }

    public void setContentHtmlPath(String contentHtmlPath) {
        this.contentHtmlPath = contentHtmlPath;
    }

    public int getAttachNum() {
        return attachNum;
    }

    public void setAttachNum(int attachNum) {
        this.attachNum = attachNum;
    }

    public int getMailCCNum() {
        return mailCCNum;
    }

    public void setMailCCNum(int mailCCNum) {
        this.mailCCNum = mailCCNum;
    }

    public int getMailToNum() {
        return mailToNum;
    }

    public void setMailToNum(int mailToNum) {
        this.mailToNum = mailToNum;
    }

    public List<MaillAttachEntry> getMaillAttachEntryList() {
        return maillAttachEntryList;
    }

    public List<String> getMailCCList() {
        return mailCCList;
    }

    public List<String> getMailToList() {
        return mailToList;
    }

    public SecMatchResult getSecMatchResult() {
        return secMatchResult;
    }

    public void setSecMatchResult(SecMatchResult secMatchResult) {
        this.secMatchResult = secMatchResult;
    }


    private class MaillAttachEntry{

        private final String name;
        private final String path;


        private MaillAttachEntry(String name, String path) {
            this.name = name;
            this.path = path;
        }

        public void toJson(XContentBuilder cb){

            try {
                XContentBuilder obj = cb.startObject();
                obj.field("name",name);
                obj.field("path",path);
                obj.endObject();

            } catch (IOException e) {
                e.printStackTrace();
            }
        }
        public String getName() {
            return name;
        }

        public String getPath() {
            return path;
        }

        public String toString(){

            StringBuffer sb = new StringBuffer();
            sb.append("attach:{");
            sb.append("name:");
            sb.append(name);
            sb.append(",");
            sb.append("path:");
            sb.append(path);
            sb.append("}");
            return sb.toString();
        }
    }

    private final static String getExtName(String fileName){

        if(TextUtils.isEmpty(fileName))
            return "data";

        int index = fileName.lastIndexOf(".");
        if(index<0)
            return "data";

        return fileName.substring(index+1);
    }

    private FileTranSession toFileTranSession(MaillAttachEntry entry){

        long fsize = 0;
        try {
            fsize = Files.size(Paths.get(entry.getPath()));
        } catch (IOException e) {
            e.printStackTrace();
        }

        FileTranSession fileTranSession = new FileTranSession();

        fileTranSession.setProto("mail");
        fileTranSession.setParentObjectId(getObjectId());
        fileTranSession.setSrcIP(IPUtils.ipv4Str(sessionEntry.getReqIP()));
        fileTranSession.setSrcPort(sessionEntry.getReqPort());
        fileTranSession.setDstIP(IPUtils.ipv4Str(sessionEntry.getResIP()));
        fileTranSession.setDstPort(sessionEntry.getResPort());
        fileTranSession.setTime(sessionEntry.getReqStartTime());
        fileTranSession.setBytes(fsize);
        fileTranSession.setContentPath(entry.getPath());
        fileTranSession.setFname(entry.getName());
        fileTranSession.setExtName(getExtName(entry.getName()));

        return fileTranSession;
    }

    @Override
    public List<SourceEntry> generate(){
        return maillAttachEntryList.stream().map(entry -> toFileTranSession(entry)).collect(Collectors.toList());
    }

    private void parseAttach(MessageUnpacker unpacker) throws IOException {

        int n = MessagePackUtil.parseArrayHeader(unpacker,true);

        String name,value;

        for(int i = 0;i<n;i++){
            MessagePackUtil.parseMapHeader(unpacker,false);
            name = MessagePackUtil.parseText(unpacker);
            value = MessagePackUtil.parseText(unpacker);

            MaillAttachEntry maillAttachEntry = new MaillAttachEntry(name,value);
            maillAttachEntryList.add(maillAttachEntry);
        }

    }

    private void parseMailSessionWithAttk(MessageUnpacker unpacker) throws IOException {

        setUserName(MessagePackUtil.parseText(unpacker));
        setPasswd(MessagePackUtil.parseText(unpacker));
        setFrom(MessagePackUtil.parseText(unpacker));
        setSubject(MessagePackUtil.parseText(unpacker));
        setContentTxtPath(MessagePackUtil.parseText(unpacker));
        setContentHtmlPath(MessagePackUtil.parseText(unpacker));
        setAttachNum(MessagePackUtil.parseInt(unpacker));
        setMailCCNum(MessagePackUtil.parseInt(unpacker));
        setMailToNum(MessagePackUtil.parseInt(unpacker));

        MessagePackUtil.parseArrayString(unpacker,true,mailCCList);
        MessagePackUtil.parseArrayString(unpacker,true,mailToList);

        /*parse attach*/
        parseAttach(unpacker);

        secMatchResult =  new SecMatchResult(unpacker);

    }

    private void parseMailSessionRaw(MessageUnpacker unpacker) throws IOException {

        setUserName(MessagePackUtil.parseText(unpacker));
        setPasswd(MessagePackUtil.parseText(unpacker));
        setSubject(MessagePackUtil.parseText(unpacker));
        setFrom(MessagePackUtil.parseText(unpacker));
        setMailToNum(MessagePackUtil.parseInt(unpacker));
        MessagePackUtil.parseArrayMap(unpacker,true,mailToList);
        setMailCCNum(MessagePackUtil.parseInt(unpacker));
        MessagePackUtil.parseArrayMap(unpacker,true,mailCCList);
        setContentTxtPath(MessagePackUtil.parseText(unpacker));
        setContentHtmlPath(MessagePackUtil.parseText(unpacker));
        setAttachNum(MessagePackUtil.parseInt(unpacker));
        parseAttach(unpacker);

    }

    public void parse(MessageUnpacker unpacker) throws IOException{

        int n = MessagePackUtil.parseMapHeader(unpacker,false);
        boolean isAttk = (n==3);

        Preconditions.checkArgument(n==2||n==3,"Invalid mail session messagePack:"+n);

        /*parse session entry */
        sessionEntry.parse(unpacker);

        /* parse mail proto info*/
        n = MessagePackUtil.parseMapHeader(unpacker,true);
        Preconditions.checkArgument(n == 12,"Invalid mail session msgpack packet:"+n);

        if(isAttk){

            parseMailSessionWithAttk(unpacker);
        }else{

            parseMailSessionRaw(unpacker);
        }
    }

    public SessionEntry getSessionEntry() {
        return sessionEntry;
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
        TextUtils.addText(sb,"userName",userName);
        TextUtils.addText(sb,"passwd",passwd);
        TextUtils.addText(sb,"subject",subject);
        TextUtils.addText(sb,"from",from);
        TextUtils.addText(sb,"contentTxtPath",contentTxtPath);
        TextUtils.addText(sb,"contentHtmlPath",contentHtmlPath);
        TextUtils.addList(sb,"mailTo",mailToList);
        TextUtils.addList(sb,"mailCC",mailCCList);
        TextUtils.addList(sb,"mailAttach",maillAttachEntryList);

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
        cb.field("userName",userName);
        cb.field("passwd",passwd);
        cb.field("subject", Text.decode(subject==null?"".getBytes():subject.getBytes()));
        cb.field("from",MailAddrUtil.getMailAddrFromString(from));
        cb.field("contentTxtPath",contentTxtPath);
        cb.field("contentHtmlPath",contentHtmlPath);

        cb.field("mailToNum",mailToNum);
        cb.field("mailTo",MailAddrUtil.toSimpleFromArray(mailToList));

        cb.field("mailCCNum",mailCCNum);
        cb.field("mailCC",MailAddrUtil.toSimpleFromArray(mailCCList));

        Set<String> sets = new TreeSet<>(mailToList);
        sets.addAll(mailCCList);

        cb.field("mailRcvList", sets.stream().map(e->MailAddrUtil.getMailAddrFromString(e)).collect(Collectors.toList()));
        cb.field("attachNum",attachNum);

        XContentBuilder attachCB = cb.startArray("mailAttach");
        maillAttachEntryList.stream().forEach(attach->attach.toJson(attachCB));
        attachCB.endArray();

        if(hasMatchSec()){
            XContentBuilder attkCB = cb.startObject("attack");
            secMatchResult.dataToJson(attkCB);
            attkCB.endObject();
        }

        return cb;
    }

    @Override
    public String getIndexMapping() {

        String mapping = "{" +
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
                "\"userName\":{\"type\":\"keyword\"}," +
                "\"passwd\":{\"type\":\"keyword\"}," +
                "\"subject\":{\"type\":\"keyword\"}," +
                "\"from\":{\"type\":\"keyword\"}," +
                "\"contentTxtPath\":{\"type\":\"keyword\"}," +
                "\"contentHtmlPath\":{\"type\":\"keyword\"}," +
                "\"contentHtmlPath\":{\"type\":\"keyword\"}," +
                "\"contentHtmlPath\":{\"type\":\"keyword\"}," +
                "\"mailToNum\":{\"type\":\"integer\"}," +
                "\"mailTo\":{\"type\":\"keyword\"}," +
                "\"mailCCNum\":{\"type\":\"integer\"}," +
                "\"mailCC\":{\"type\":\"keyword\"}," +
                "\"mailRcvList\":{\"type\":\"keyword\"}," +
                "\"attachNum\":{\"type\":\"integer\"}," +
                "\"mailAttach\":{" +
                "\"properties\":{" +
                "\"name\":{\"type\":\"keyword\"}," +
                "\"path\":{\"type\":\"keyword\"}" +
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
        return "log_tcp_session_mail";
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


}
