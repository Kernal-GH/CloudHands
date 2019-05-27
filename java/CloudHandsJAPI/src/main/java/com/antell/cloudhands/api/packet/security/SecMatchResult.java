package com.antell.cloudhands.api.packet.security;

import com.antell.cloudhands.api.DataDump;
import com.antell.cloudhands.api.sink.es.ESIndexable;
import com.antell.cloudhands.api.utils.MessagePackUtil;
import org.elasticsearch.common.xcontent.XContentBuilder;
import org.msgpack.core.MessageUnpacker;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

/**
 * Created by dell on 2018/6/11.
 */
public class SecMatchResult implements ESIndexable,DataDump{

    private int matchCount;
    private final List<MatchInfo> matchInfoList;


    public SecMatchResult(MessageUnpacker unpacker) throws IOException {
        this.matchCount = 0;
        this.matchInfoList = new ArrayList<>();

        parse(unpacker);

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

    private void parse(MessageUnpacker unpacker) throws IOException {

        int n  = MessagePackUtil.parseArrayHeader(unpacker,true);
        for(int i = 0;i<n;i++){

            MatchInfo matchInfo = new MatchInfo();
            matchInfo.parse(unpacker);
            addMatchInfo(matchInfo);
        }
    }

    @Override
    public String dataToString() {

        StringBuffer sb = new StringBuffer();
        sb.append("Dump match info:\n\n");
        for(MatchInfo matchInfo:matchInfoList){

            sb.append(matchInfo.dataToString());
            sb.append("\n\n");
        }

        return sb.toString();
    }

    public MatchInfo getMainMatchInfo(){

        long minLevel = Long.MAX_VALUE;
        MatchInfo mainMatchInof = null;
        for(MatchInfo matchInfo:matchInfoList){

            if(matchInfo.getRuleLevel()<minLevel){
                minLevel = matchInfo.getRuleLevel();
                mainMatchInof = matchInfo;
            }
        }

        return mainMatchInof;
    }

    @Override
    public XContentBuilder dataToJson(XContentBuilder cb) throws IOException {

        MatchInfo mainMatchInfo = getMainMatchInfo();

        mainMatchInfo.mainDataToJson(cb);

        XContentBuilder matchCB = cb.startArray("matchInfoList");
        for(MatchInfo matchInfo:matchInfoList){
            if(matchInfo == mainMatchInfo)
                continue;

            matchInfo.dataToJson(matchCB);
        }

        matchCB.endArray();

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

}
