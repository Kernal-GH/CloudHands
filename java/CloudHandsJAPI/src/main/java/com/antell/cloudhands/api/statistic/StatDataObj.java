package com.antell.cloudhands.api.statistic;

import com.antell.cloudhands.api.utils.Constants;
import com.antell.cloudhands.api.utils.TextUtils;

import javax.xml.soap.Text;
import java.io.DataInput;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

/**
 * Created by dell on 2018/6/8.
 */
public class StatDataObj {

    private String type;
    private List<StatDataItem> items;

    public StatDataObj(){

        type = "";
        items = new ArrayList<>();
    }

    public void build(DataInput din,long startTime,long tv) throws IOException {

        setType(Constants.statDataType2String[din.readUnsignedShort()]);
        int n = din.readInt();
        for(int i = 0;i<n;i++){

            StatDataItem item = new StatDataItem();
            item.setTime(startTime+tv*i);
            item.setPackets(din.readLong());
            item.setBytes(din.readLong());
            items.add(item);
        }
    }

    public String getType() {
        return type;
    }

    public void setType(String type) {
        this.type = type;
    }

    public List<StatDataItem> getItems() {
        return items;
    }

    public void setItems(List<StatDataItem> items) {
        this.items = items;
    }

    public void dump(StringBuffer sb){

        TextUtils.addText(sb,"type",type);
        sb.append("items:[");
        for(StatDataItem item : items){

            sb.append("(");
            sb.append(item.getTime()+",");
            sb.append(item.getPackets()+",");
            sb.append(item.getBytes());
            sb.append("),");
        }
        sb.append("]\n");
    }
}
