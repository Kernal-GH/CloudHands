package com.antell.cloudhands.api.packet.udp.dns;

import com.antell.cloudhands.api.BinDataInput;
import com.antell.cloudhands.api.DataDump;
import com.antell.cloudhands.api.MsgPackDataInput;
import com.antell.cloudhands.api.sink.es.ESIndexable;
import com.antell.cloudhands.api.utils.MessagePackUtil;
import com.antell.cloudhands.api.utils.TextUtils;
import com.google.common.base.Preconditions;
import org.elasticsearch.common.xcontent.XContentBuilder;
import org.msgpack.core.MessageUnpacker;

import java.io.DataInput;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

/**
 * Created by dell on 2018/6/15.
 */
public class DNSRequst implements BinDataInput,MsgPackDataInput,ESIndexable,DataDump{

    private Header header;
    private List<DNSQuestion> questions;


    @Override
    public void read(DataInput in) throws IOException {

        header = new Header(in);
        questions = new ArrayList<>();
        int n = header.getCount(Section.QUESTION);

        for(int i=0;i<n;i++){

            DNSQuestion question = new DNSQuestion();
            question.read(in);

            addQuestion(question);

        }
    }

    @Override
    public void parse(MessageUnpacker unpacker) throws IOException {

        int n = MessagePackUtil.parseMapHeader(unpacker,true);
        Preconditions.checkArgument(n==2,"Invalid msgpack packet of udp session request entry:"+n);
        header = new Header(unpacker);
        questions = new ArrayList<>();

        n = MessagePackUtil.parseArrayHeader(unpacker,true);

        for(int i=0;i<n;i++){

            DNSQuestion question = new DNSQuestion();
            question.parse(unpacker);
            addQuestion(question);
        }

    }

    @Override
    public XContentBuilder dataToJson(XContentBuilder cb) throws IOException {

        XContentBuilder reqHCB = cb.startObject("reqHeader");
        header.toJson(reqHCB);
        reqHCB.endObject();

        XContentBuilder reqQCB = cb.startArray("reqQuestions");
        for(DNSQuestion question: questions){

            XContentBuilder qcb = reqQCB.startObject();
            question.dataToJson(qcb);
            qcb.endObject();
        }
        reqQCB.endArray();

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
        sb.append("dns.request.info\n\n");
        sb.append(header.toString());

        for(DNSQuestion question:questions){

            sb.append(question.dataToString());
            sb.append("\n\n");
        }
        return sb.toString();
    }

    public List<DNSQuestion> getQuestions() {
        return questions;
    }

    public String getDomain(){

        if(questions == null ||questions.isEmpty())
            return "";

        for(DNSQuestion question:questions){

            Name name = question.getName();
            if(name == null)
                continue;

            String dm = name.getName();
            if(!TextUtils.isEmpty(dm))
                return dm;
        }

        return "";
    }



    public void addQuestion(DNSQuestion question) {

        questions.add(question);
    }
}
