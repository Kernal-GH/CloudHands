package com.antell.cloudhands.api.packet.tcp.mail;

import com.antell.cloudhands.api.packet.SessionEntry;
import com.antell.cloudhands.api.packet.security.SecMatchResult;
import com.antell.cloudhands.api.utils.MessagePackUtil;
import com.google.common.base.Preconditions;
import org.msgpack.core.MessageUnpacker;

import java.io.DataInput;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

/**
 * Created by dell on 2018/6/11.
 */
public class MailSession extends SessionEntry {

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

    public MailSession() {
        this.maillAttachEntryList = new ArrayList<>();
        this.mailCCList = new ArrayList<>();
        this.mailToList = new ArrayList<>();
        this.secMatchResult = null;
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

    }

    @Override
    public void read(DataInput in) throws IOException {

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

        secMatchResult =  new SecMatchResult();
        secMatchResult.parse(unpacker);

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

    @Override
    public void parse(MessageUnpacker unpacker) throws IOException{

        int n = MessagePackUtil.parseMapHeader(unpacker,false);
        boolean isAttk = (n==3);

        Preconditions.checkArgument(n==2||n==3,"Invalid mail session messagePack:"+n);

        /*parse session entry */
        super.parse(unpacker);

        /* parse mail proto info*/
        n = MessagePackUtil.parseMapHeader(unpacker,true);
        Preconditions.checkArgument(n == 12,"Invalid mail session msgpack packet:"+n);

        if(isAttk){

            parseMailSessionWithAttk(unpacker);
        }else{

            parseMailSessionRaw(unpacker);
        }
    }



}
