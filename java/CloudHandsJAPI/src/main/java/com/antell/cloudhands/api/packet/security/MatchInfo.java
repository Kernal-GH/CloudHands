package com.antell.cloudhands.api.packet.security;

import com.antell.cloudhands.api.DataDump;
import com.antell.cloudhands.api.MsgPackDataInput;
import com.antell.cloudhands.api.sink.es.ESIndexable;
import com.antell.cloudhands.api.utils.MessagePackUtil;
import com.antell.cloudhands.api.utils.TextUtils;
import com.google.common.base.Preconditions;
import org.elasticsearch.common.xcontent.XContentBuilder;
import org.msgpack.core.MessageUnpacker;

import java.io.IOException;


/**
 * Created by dell on 2018/6/11.
 */
public class MatchInfo implements MsgPackDataInput, ESIndexable,DataDump{

    private String engineName;
    private int engineType;
    private int engineLevel;
    private long ruleID;
    private long ruleLevel;
    private String ruleMsg;
    private String ruleType;
    private String rulePayload;
    private String ruleVarName;
    private String ruleVarValue;

    @Override
    public void parse(MessageUnpacker unpacker) throws IOException {

        int n = MessagePackUtil.parseMapHeader(unpacker,false);
        Preconditions.checkArgument(n == 10,"Invalid Msgpack packet of security match information:"+n);

        setEngineName(MessagePackUtil.parseText(unpacker));
        setEngineType(MessagePackUtil.parseInt(unpacker));
        setEngineLevel(MessagePackUtil.parseInt(unpacker));
        setRuleID(MessagePackUtil.parseLong(unpacker));
        setRuleLevel(MessagePackUtil.parseLong(unpacker));
        setRuleMsg(MessagePackUtil.parseText(unpacker));
        setRuleType(MessagePackUtil.parseText(unpacker));
        setRulePayload(MessagePackUtil.parseText(unpacker));
        setRuleVarName(MessagePackUtil.parseText(unpacker));
        setRuleVarValue(MessagePackUtil.parseText(unpacker));

    }

    @Override
    public String dataToString() {

        StringBuffer sb = new StringBuffer();

        TextUtils.addText(sb,"engineName",engineName);
        TextUtils.addInt(sb,"engineType",engineType);
        TextUtils.addInt(sb,"engineLevel",engineLevel);
        TextUtils.addLong(sb,"ruleID",ruleID);
        TextUtils.addLong(sb,"ruleLevel",ruleLevel);
        TextUtils.addText(sb,"ruleMsg",ruleMsg);
        TextUtils.addText(sb,"ruleType",ruleType);
        TextUtils.addText(sb,"rulePayload",rulePayload);
        TextUtils.addText(sb,"ruleVarName",ruleVarName);
        TextUtils.addText(sb,"ruleVarValue",ruleVarValue);

        return sb.toString();
    }

    @Override
    public XContentBuilder dataToJson(XContentBuilder cb) throws IOException {

       cb.field("engineName",engineName);
       cb.field("engineType",engineType);
       cb.field("engineLevel",engineLevel);
       cb.field("ruleID",ruleID);
       cb.field("ruleLevel",ruleLevel);
       cb.field("ruleMsg",ruleMsg);
       cb.field("ruleType",ruleType);
       cb.field("rulePayload",rulePayload);
       cb.field("ruleVarName",ruleVarName);
       cb.field("ruleVarValue",ruleVarValue);

        return cb;
    }

    @Override
    public String getIndexMapping() {
        return null;
    }

    @Override
    public String getIndexNamePrefix() {
        return null;
    }

    @Override
    public String getIndexDocType() {
        return null;
    }

    public XContentBuilder mainDataToJson(XContentBuilder cb) throws IOException {

        cb.field("mainEngineName",engineName);
        cb.field("mainEngineType",engineType);
        cb.field("mainEngineLevel",engineLevel);
        cb.field("mainRuleID",ruleID);
        cb.field("mainRuleLevel",ruleLevel);
        cb.field("mainRuleMsg",ruleMsg);
        cb.field("mainRuleType",ruleType);
        cb.field("mainRulePayload",rulePayload);
        cb.field("mainRuleVarName",ruleVarName);
        cb.field("mainRuleVarValue",ruleVarValue);
        return cb;
    }

    public String getEngineName() {
        return engineName;
    }

    public void setEngineName(String engineName) {
        this.engineName = engineName;
    }

    public int getEngineType() {
        return engineType;
    }

    public void setEngineType(int engineType) {
        this.engineType = engineType;
    }

    public int getEngineLevel() {
        return engineLevel;
    }

    public void setEngineLevel(int engineLevel) {
        this.engineLevel = engineLevel;
    }

    public long getRuleID() {
        return ruleID;
    }

    public void setRuleID(long ruleID) {
        this.ruleID = ruleID;
    }

    public long getRuleLevel() {
        return ruleLevel;
    }

    public void setRuleLevel(long ruleLevel) {
        this.ruleLevel = ruleLevel;
    }

    public String getRuleMsg() {
        return ruleMsg;
    }

    public void setRuleMsg(String ruleMsg) {
        this.ruleMsg = ruleMsg;
    }

    public String getRuleType() {
        return ruleType;
    }

    public void setRuleType(String ruleType) {
        this.ruleType = ruleType;
    }

    public String getRulePayload() {
        return rulePayload;
    }

    public void setRulePayload(String rulePayload) {
        this.rulePayload = rulePayload;
    }

    public String getRuleVarName() {
        return ruleVarName;
    }

    public void setRuleVarName(String ruleVarName) {
        this.ruleVarName = ruleVarName;
    }

    public String getRuleVarValue() {
        return ruleVarValue;
    }

    public void setRuleVarValue(String ruleVarValue) {
        this.ruleVarValue = ruleVarValue;
    }

}
