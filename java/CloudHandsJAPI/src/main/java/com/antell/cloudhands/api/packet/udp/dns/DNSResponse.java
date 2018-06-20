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
public class DNSResponse implements BinDataInput,DataOutJson,DataDump{


    private Header header;
    private List[] sections;



    private void buildRecord(List records,DataInput in,int count) throws IOException {

        for(int i = 0;i<count;i++){

            Record record = Record.build(in);
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

    private XContentBuilder recordsToJson(XContentBuilder cb,int i) throws IOException {


        String fname = Section.longString(i);

        List<Record> records = getRecords(i);
        if(records == null)
            return cb;

        XContentBuilder cbb = cb.startArray(fname);

        for (Record record :records) {
            XContentBuilder cbo = cbb.startObject();
            record.toJson(cbo);
            cbo.endObject();
        }

        cbb.endArray();

        return cb;
    }

    private XContentBuilder questionsToJson(XContentBuilder cb) throws IOException {

        String fname = Section.longString(Section.QUESTION);
        List<DNSQuestion> questions = getQuestions();
        if(questions == null)
            return cb;

        XContentBuilder cbb = cb.startArray(fname);

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

            XContentBuilder rdatacb = cb.startArray("resRDatas");
            for (int i = 0; i < 4; i++) {

                if(i == Section.QUESTION)
                    questionsToJson(cb);
                else
                    recordsToJson(rdatacb,i);
            }

            rdatacb.endArray();
            return cb;
        } catch (Exception e) {
            e.printStackTrace();
            return null;
        }
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
