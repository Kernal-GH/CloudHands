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
import java.util.ArrayList;
import java.util.List;

/**
 * Created by dell on 2018/6/15.
 */
public class DNSResponse implements BinDataInput,MsgPackDataInput, ESIndexable,DataDump{


    private Header header;
    private List[] sections;



    private void buildRecord(List records,DataInput in,int count) throws IOException {

        for(int i = 0;i<count;i++){

            Record record = Record.build(in);
            records.add(record);
        }
    }
    private void buildRecord(List records,MessageUnpacker unpacker) throws IOException {

        MessagePackUtil.parseMapHeader(unpacker,true);
        int count = MessagePackUtil.parseArrayHeader(unpacker,true);

        for(int i = 0;i<count;i++){

            Record record = Record.build(unpacker);
            records.add(record);
        }
    }

    private void buildQuestion(List records,DataInput in,int count) throws IOException {

        for(int i = 0;i<count;i++){

            DNSQuestion question = new DNSQuestion();
            question.read(in);
            records.add(question);
        }
    }

    private void buildQuestion(List records,MessageUnpacker unpacker,int count) throws IOException {

        for(int i = 0;i<count;i++){

            DNSQuestion question = new DNSQuestion();
            question.parse(unpacker);
            records.add(question);
        }
    }

    @Override
    public void read(DataInput in) throws IOException {

        header = new Header(in);
        sections = new List[4];

        try {
            for (int i = 0; i < 4; i++) {
                int count = header.getCount(i);
                if (count > 0)
                    sections[i] = new ArrayList(count);

                if(i == Section.QUESTION)
                    buildQuestion(sections[i],in,count);
                else
                    buildRecord(sections[i],in,count);
            }
        } catch (ParseException e) {
                throw e;
        }
    }

    @Override
    public void parse(MessageUnpacker unpacker) throws IOException {

        int n = MessagePackUtil.parseMapHeader(unpacker,true);
        Preconditions.checkArgument(n==5,"Invalid msgpack packet of udp session response entry:"+n);
        header = new Header(unpacker);
        sections = new List[4];

        sections[0] = new ArrayList();
        sections[1] = new ArrayList();
        sections[2] = new ArrayList();
        sections[3] = new ArrayList();
        
        buildQuestion(sections[Section.QUESTION],unpacker,MessagePackUtil.parseArrayHeader(unpacker,true));
        buildRecord(sections[Section.ANSWER],unpacker);
        buildRecord(sections[Section.AUTHORITY],unpacker);
        buildRecord(sections[Section.ADDITIONAL],unpacker);

    }

    private XContentBuilder recordsToJson(XContentBuilder cb,int i) throws IOException {


        String fname = Section.longString(i);


        XContentBuilder cbb = cb.startArray(fname);
        List<Record> records = getRecords(i);
        if(records == null) {
            cbb.endArray();
            return cb;
        }

        for (Record record :records) {
            XContentBuilder cbo = cbb.startObject();
            record.toJson(cbo);
            cbo.endObject();
        }

        cbb.endArray();

        return cb;
    }

    public List<String> getIPV4Adresses(){

        List<String> results = new ArrayList<>();
        List<Record> records = getRecords(Section.ANSWER);

        if(records!=null){

            records.forEach(record -> {

                if(record instanceof ARecord){

                    ARecord aRecord = (ARecord)record;
                    results.add(aRecord.rrToString());
                }
            });
        }

        return results;
    }

    private XContentBuilder questionsToJson(XContentBuilder cb) throws IOException {

        String fname = Section.longString(Section.QUESTION);


        XContentBuilder cbb = cb.startArray(fname);

        List<DNSQuestion> questions = getQuestions();
        if(questions == null)
        {
            cbb.endArray();
            return cb;
        }

        for (DNSQuestion question :questions) {
            XContentBuilder cbo = cbb.startObject();
            question.dataToJson(cbo);
            cbo.endObject();
        }

        cbb.endArray();

        return cb;
    }

    @Override
    public XContentBuilder dataToJson(XContentBuilder cb) throws IOException {

        try {

            XContentBuilder hcb = cb.startObject("resHeader");
            header.toJson(hcb);
            hcb.endObject();

            questionsToJson(cb);

            for (int i = 1; i < 4; i++) {

                recordsToJson(cb,i);
            }

        } catch (Exception e) {
            e.printStackTrace();
        }

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

    public List getQuestions(){

        return sections[Section.QUESTION];
    }

    public List getRecords(int i){

        if(i <0||i>=4)
            return null;

        return sections[i];
    }

    public Header getHeader() {
        return header;
    }

    private void questionsToString(StringBuffer sb){

        sb.append(Section.longString(Section.QUESTION)+"---------->>\n");
        List<DNSQuestion> dnsQuestions = getQuestions();
        if(dnsQuestions == null)
            return;

        for(DNSQuestion question:dnsQuestions){

            sb.append(question.dataToString());
            sb.append("\n\n");
        }
    }

    private void recordsToString(StringBuffer sb,int i){

        sb.append(Section.longString(i)+"---------->>\n");
        List<Record> records = getRecords(i);
        if(records == null)
            return;

        for(Record record:records){
            sb.append(record.toString());
            sb.append("\n\n");
        }
    }

    @Override
    public String dataToString() {

        StringBuffer sb = new StringBuffer();
        sb.append(header + "\n");

        for (int i = 0; i < 4; i++) {
            if (i == Section.QUESTION)
                questionsToString(sb);
            else
                recordsToString(sb,i);
        }

        return sb.toString();
    }


}
