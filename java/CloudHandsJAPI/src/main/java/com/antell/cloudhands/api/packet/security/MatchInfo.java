package com.antell.cloudhands.api.packet.security;

import com.antell.cloudhands.api.packet.DataReadable;
import com.antell.cloudhands.api.utils.MessagePackUtil;
import com.google.common.base.Preconditions;
import org.msgpack.core.MessageUnpacker;

import java.io.DataInput;
import java.io.IOException;


/**
 * Created by dell on 2018/6/11.
 */
public class MatchInfo implements DataReadable {

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
    public void read(DataInput in) throws IOException {

    }

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
