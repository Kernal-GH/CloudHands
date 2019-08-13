package com.antell.cloudhands.api.packet.udp.dns;

import com.antell.cloudhands.api.utils.Base64;
import com.antell.cloudhands.api.utils.MessagePackUtil;
import com.antell.cloudhands.api.utils.Text;
import org.elasticsearch.common.xcontent.XContentBuilder;
import org.msgpack.core.MessageUnpacker;

import java.io.DataInput;
import java.io.IOException;
import java.net.Inet6Address;
import java.net.InetAddress;

/**
 * IPsec Keying Material (RFC 4025)
 */

public class IPSECKEYRecord extends Record {

    public static class Algorithm {
        private Algorithm() {
        }

        public static final int DSA = 1;
        public static final int RSA = 2;
    }

    public static class Gateway {
        private Gateway() {
        }

        public static final int None = 0;
        public static final int IPv4 = 1;
        public static final int IPv6 = 2;
        public static final int Name = 3;
    }
    private int precedence;
    private int gatewayType;
    private int algorithmType;
    private Object gateway;
    private byte[] key;

    public IPSECKEYRecord() {
    }

    @Override
    public Record getObject() {
        return new IPSECKEYRecord();
    }

    @Override
    public void read(DataInput in) throws IOException {
        precedence = in.readUnsignedByte();
        gatewayType = in.readUnsignedByte();
        algorithmType = in.readUnsignedByte();

        switch (gatewayType) {
            case Gateway.None:
                gateway = null;
                break;
            case Gateway.IPv4:
                byte[] data = new byte[4];
                in.readFully(data,0,4);
                gateway = InetAddress.getByAddress(data);
                break;
            case Gateway.IPv6:
                gateway = InetAddress.getByAddress(Text.readBytes(in,2));

                break;
            case Gateway.Name:
                gateway = new Name(in);
                break;

            default:
                throw new ParseException("invalid gateway type");
        }

        key = Text.readBytes(in,2);

    }

    @Override
    public void read(MessageUnpacker unpacker) throws IOException {

        MessagePackUtil.parseMapHeader(unpacker,true);
        precedence = MessagePackUtil.parseInt(unpacker);
        gatewayType = MessagePackUtil.parseInt(unpacker);
        algorithmType = MessagePackUtil.parseInt(unpacker);
        switch (gatewayType) {
            case Gateway.IPv4:
                gateway = InetAddress.getByAddress(MessagePackUtil.parseBin(unpacker));
                break;
            case Gateway.IPv6:
                gateway = InetAddress.getByAddress(MessagePackUtil.parseBin(unpacker));
                break;
            case Gateway.Name:
                gateway = new Name(unpacker);
                break;

            default:
                MessagePackUtil.parseInt(unpacker);
        }

        key = MessagePackUtil.parseBin(unpacker);
    }

    @Override
    public String rrToString() {
        StringBuffer sb = new StringBuffer();
        sb.append(precedence);
        sb.append(" ");
        sb.append(gatewayType);
        sb.append(" ");
        sb.append(algorithmType);
        sb.append(" ");
        switch (gatewayType) {
            case Gateway.None:
                sb.append(".");
                break;
            case Gateway.IPv4:
            case Gateway.IPv6:
                InetAddress gatewayAddr = (InetAddress) gateway;
                sb.append(gatewayAddr.getHostAddress());
                break;
            case Gateway.Name:
                sb.append(gateway);
                break;
        }
        if (key != null) {
            sb.append(" ");
            sb.append(Base64.toString(key));
        }
        return sb.toString();
    }

    @Override
    public XContentBuilder rdataToJson(XContentBuilder cb) throws IOException {

        cb.field("precedence",precedence);
        cb.field("gatewayType",gatewayType);
        cb.field("algorithmType",algorithmType);

        String gw = "";
        switch (gatewayType){

            case Gateway.None:
                gw = ".";
                break;
            case Gateway.IPv4:
            case Gateway.IPv6:
                InetAddress gatewayAddr = (InetAddress) gateway;
                gw = gatewayAddr.getHostAddress();
                break;
            case Gateway.Name:
                gw = (String) gateway;
                break;
        }

        cb.field("gateway",gw);

        cb.field("key",key == null?"":Base64.toString(key));
        return cb;
    }

    /**
     * Returns the record's precedence.
     */
    public int
    getPrecedence() {
        return precedence;
    }

    /**
     * Returns the record's gateway type.
     */
    public int
    getGatewayType() {
        return gatewayType;
    }

    /**
     * Returns the record's algorithm type.
     */
    public int
    getAlgorithmType() {
        return algorithmType;
    }

    /**
     * Returns the record's gateway.
     */
    public Object
    getGateway() {
        return gateway;
    }

    /**
     * Returns the record's public key
     */
    public byte[]
    getKey() {
        return key;
    }


}
