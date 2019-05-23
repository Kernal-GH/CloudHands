package com.antell.cloudhands.api.packet.udp.dns;

import org.elasticsearch.common.xcontent.XContentBuilder;

import java.io.DataInput;
import java.io.IOException;

/**
 * Mailbox information Record - lists the address responsible for a mailing
 * list/mailbox and the address to receive error messages relating to the
 * mailing list/mailbox.
 */

public class MINFORecord extends Record {

    private Name responsibleAddress;
    private Name errorAddress;

    public MINFORecord() {
    }


    @Override
    public Record getObject() {
        return new MINFORecord();
    }

    @Override
    public void read(DataInput in) throws IOException {
        responsibleAddress = new Name(in);
        errorAddress = new Name(in);
    }

    /**
     * Converts the MINFO Record to a String
     */
    @Override
    public String rrToString() {
        StringBuffer sb = new StringBuffer();
        sb.append(responsibleAddress);
        sb.append(" ");
        sb.append(errorAddress);
        return sb.toString();
    }

    @Override
    public XContentBuilder rdataToJson(XContentBuilder cb) throws IOException {

        cb.field("responsibleAddress",responsibleAddress.toString());
        cb.field("errorAddress",errorAddress.toString());

        return cb;
    }

    /**
     * Gets the address responsible for the mailing list/mailbox.
     */
    public Name getResponsibleAddress() {
        return responsibleAddress;
    }

    /**
     * Gets the address to receive error messages relating to the mailing
     * list/mailbox.
     */
    public Name getErrorAddress() {
        return errorAddress;
    }


}
