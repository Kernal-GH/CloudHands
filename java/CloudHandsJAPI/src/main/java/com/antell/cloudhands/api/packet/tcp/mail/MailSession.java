package com.antell.cloudhands.api.packet.tcp.mail;

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
public class MailSession implements SourceEntry {

    private TCPSessionEntry sessionEntry;

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

    public TCPSessionEntry getSessionEntry() {
        return sessionEntry;
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


        cb.field("userName",userName);
        cb.field("passwd",passwd);
        cb.field("subject",subject);
        cb.field("from",from);
        cb.field("contentTxtPath",contentTxtPath);
        cb.field("contentHtmlPath",contentHtmlPath);
        cb.field("mailTo",mailToList);
        cb.field("mailCC",mailCCList);
        cb.field("mailAttach",maillAttachEntryList);

        if(secMatchResult!=null){
            XContentBuilder attkCB = cb.startObject("attack");
            secMatchResult.dataToJson(attkCB);
            attkCB.endObject();
        }

        return cb;
    }


}
