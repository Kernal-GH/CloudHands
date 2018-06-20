package com.antell.cloudhands.api.packet.udp.dns;

import com.antell.cloudhands.api.BinDataInput;
import com.antell.cloudhands.api.DataDump;
import com.antell.cloudhands.api.DataOutJson;
import org.elasticsearch.common.xcontent.XContentBuilder;

import java.io.DataInput;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

/**
 * Created by dell on 2018/6/15.
 */
public class DNSRequst implements BinDataInput,DataOutJson,DataDump{

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

    public void addQuestion(DNSQuestion question) {

        questions.add(question);
    }
}
