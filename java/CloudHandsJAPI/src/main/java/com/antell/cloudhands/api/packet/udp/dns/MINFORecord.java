package com.antell.cloudhands.api.packet.udp.dns;

import org.elasticsearch.common.xcontent.XContentBuilder;

import java.io.IOException;

/**
 * Mailbox information Record - lists the address responsible for a mailing
 * list/mailbox and the address to receive error messages relating to the
 * mailing list/mailbox.
 */

public class MINFORecord extends Record {

    private static final long serialVersionUID = -3962147172340353796L;

    private Name responsibleAddress;
    private Name errorAddress;

    public MINFORecord() {
    }


    @Override
    public Record getObject() {
        return new MINFORecord();
    }

    /**
     * Creates an MINFO Record from the given data
     *
     * @param responsibleAddress The address responsible for the
     *                           mailing list/mailbox.
     * @param errorAddress       The address to receive error messages relating to the
     *                           mailing list/mailbox.
     */
    public MINFORecord(Name name, int dclass, long ttl,
                       Name responsibleAddress, Name errorAddress) {
        super(name, Type.MINFO, dclass, ttl);

        this.responsibleAddress = checkName("responsibleAddress",
                responsibleAddress);
        this.errorAddress = checkName("errorAddress", errorAddress);
    }

    @Override
    public void rrFromWire(DNSInput in) throws IOException {
        responsibleAddress = new Name(in);
        errorAddress = new Name(in);
    }

    @Override
    public void rdataFromString(Tokenizer st, Name origin) throws IOException {
        responsibleAddress = st.getName(origin);
        errorAddress = st.getName(origin);
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

    @Override
    public void rrToWire(DNSOutput out, Compression c, boolean canonical) {
        responsibleAddress.toWire(out, null, canonical);
        errorAddress.toWire(out, null, canonical);
    }

}
