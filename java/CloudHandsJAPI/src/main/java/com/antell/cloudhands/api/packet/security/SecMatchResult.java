package com.antell.cloudhands.api.packet.security;

import com.antell.cloudhands.api.packet.DataReadable;
import com.antell.cloudhands.api.utils.MessagePackUtil;
import org.msgpack.core.MessageUnpacker;

import java.io.DataInput;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

/**
 * Created by dell on 2018/6/11.
 */
public class SecMatchResult implements DataReadable{

    private int matchCount;
    private final List<MatchInfo> matchInfoList;


    public SecMatchResult() {
        this.matchCount = 0;
        this.matchInfoList = new ArrayList<>();
    }

    public int getMatchCount() {
        return matchCount;
    }

    public List<MatchInfo> getMatchInfoList() {
        return matchInfoList;
    }


    public  void addMatchInfo(MatchInfo matchInfo){

        matchInfoList.add(matchInfo);
        matchCount+=1;
    }

    @Override
    public void read(DataInput in) throws IOException {

    }

    @Override
    public void parse(MessageUnpacker unpacker) throws IOException {

        int n  = MessagePackUtil.parseArrayHeader(unpacker,true);

        for(int i = 0;i<n;i++){

            MatchInfo matchInfo = new MatchInfo();
            matchInfo.parse(unpacker);
            addMatchInfo(matchInfo);
        }
    }
}
