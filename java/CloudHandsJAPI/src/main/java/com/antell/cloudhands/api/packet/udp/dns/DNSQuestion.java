package com.antell.cloudhands.api.packet.udp.dns;


import com.antell.cloudhands.api.BinDataInput;
import com.antell.cloudhands.api.DataDump;
import com.antell.cloudhands.api.MsgPackDataInput;
import com.antell.cloudhands.api.sink.es.ESIndexable;
import com.antell.cloudhands.api.utils.MessagePackUtil;
import com.google.common.base.Preconditions;
import org.elasticsearch.common.xcontent.XContentBuilder;
import org.msgpack.core.MessageUnpacker;

import java.io.DataInput;
import java.io.IOException;

/**
 * Created by dell on 2018/6/15.
 */
public class DNSQuestion implements BinDataInput,MsgPackDataInput, ESIndexable,DataDump{

    private Name name;
    private int type;
    private int dclass;

    @Override
    public void read(DataInput in) throws IOException {

        name = new Name(in);
        type = in.readUnsignedShort();
        dclass = in.readUnsignedShort();
    }

    @Override
    public void parse(MessageUnpacker unpacker) throws IOException {

        int n = MessagePackUtil.parseMapHeader(unpacker,true);
        Preconditions.checkArgument(n==3,"Invalid msgpack packet of udp session question entry:"+n);

        name = new Name(unpacker);
        type = MessagePackUtil.parseInt(unpacker);
        dclass = MessagePackUtil.parseInt(unpacker);
    }

    @Override
    public XContentBuilder dataToJson(XContentBuilder cb) throws IOException {

        cb.field("name",name.getName());
        cb.field("typeStr",Type.string(type));
        cb.field("type",type);
        cb.field("classStr",DClass.string(dclass));
        cb.field("class",dclass);

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

    @Override
    public String dataToString() {

        StringBuffer sb = new StringBuffer();
        sb.append(name);
        if (sb.length() < 8)
            sb.append("\t");
        if (sb.length() < 16)
            sb.append("\t");
        sb.append("\t");
        sb.append(DClass.string(dclass));
        sb.append("\t");

        sb.append(Type.string(type));
        return sb.toString();

    }

    public Name getName() {
        return name;
    }



}
