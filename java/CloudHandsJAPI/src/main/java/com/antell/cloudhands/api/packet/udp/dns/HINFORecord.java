package com.antell.cloudhands.api.packet.udp.dns;

import com.antell.cloudhands.api.utils.Text;
import org.elasticsearch.common.xcontent.XContentBuilder;

import java.io.DataInput;
import java.io.IOException;

/**
 * Host Information - describes the CPU and OS of a host
 */

public class HINFORecord extends Record {

    private String cpu;
    private String os;

    public HINFORecord() {
    }

    @Override
    public Record getObject() {
        return new HINFORecord();
    }


    @Override
    public void read(DataInput in) throws IOException {
        cpu = Text.readString(in,2);
        os = Text.readString(in,2);

    }

    /**
     * Returns the host's CPU
     */
    public String getCPU() {

        return cpu;
    }

    /**
     * Returns the host's OS
     */
    public String getOS() {

        return os;
    }

    /**
     * Converts to a string
     */
    @Override
    public String rrToString() {
        StringBuffer sb = new StringBuffer();
        sb.append(cpu);
        sb.append(" ");
        sb.append(os);
        return sb.toString();
    }

    @Override
    public XContentBuilder rdataToJson(XContentBuilder cb) throws IOException {

        cb.field("cpu",cpu);
        cb.field("os",os);

        return cb;
    }

}
