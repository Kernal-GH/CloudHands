package com.antell.cloudhands.api.packet.udp.dns;

import java.io.DataInput;
import java.io.IOException;
import java.net.InetAddress;
import java.net.UnknownHostException;

/**
 * Address Record - maps a domain name to an Internet address
 */

public class ARecord extends Record {


    private int addr;

    public ARecord() {
    }

    private static final byte[] toArray(int addr) {
        byte[] bytes = new byte[4];
        bytes[0] = (byte) ((addr >>> 24) & 0xFF);
        bytes[1] = (byte) ((addr >>> 16) & 0xFF);
        bytes[2] = (byte) ((addr >>> 8) & 0xFF);
        bytes[3] = (byte) (addr & 0xFF);
        return bytes;
    }

    @Override
    public Record getObject() {
        return new ARecord();
    }


    @Override
    public void read(DataInput in) throws IOException {

        addr = in.readInt();
    }


    /**
     * Converts rdata to a String
     */
    @Override
    public String rrToString() {
        return (Address.toDottedQuad(toArray(addr)));
    }

    /**
     * Returns the Internet address
     */
    public InetAddress getAddress() {
        try {
            if (name == null)
                return InetAddress.getByAddress(toArray(addr));
            else
                return InetAddress.getByAddress(name.toString(),
                        toArray(addr));
        } catch (UnknownHostException e) {
            return null;
        }
    }

}
