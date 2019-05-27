package com.antell.cloudhands.api.packet;


import java.io.DataInput;
import java.io.IOException;

/**
 * Created by dell on 2018/4/20.
 */
public class MacAddress {

    private byte[] addr;

    public MacAddress(DataInput in) throws IOException {

        addr = new byte[6];
        addr[0] = (byte) in.readUnsignedByte();
        addr[1] = (byte) in.readUnsignedByte();
        addr[2] = (byte) in.readUnsignedByte();
        addr[3] = (byte) in.readUnsignedByte();
        addr[4] = (byte) in.readUnsignedByte();
        addr[5] = (byte) in.readUnsignedByte();
    }

    public byte[] getAddr() {
        return addr;
    }

    public  String toMacStr() {

        StringBuffer sb = new StringBuffer();
        String v;
        for(int i = 0;i<6;i++){
            v = Integer.toHexString(addr[i]&0xff);
            if(v.length()<2){
                sb.append(0);
            }

            sb.append(v);

            if(i<5)
                sb.append(":");
        }

        return sb.toString();
    }


}
