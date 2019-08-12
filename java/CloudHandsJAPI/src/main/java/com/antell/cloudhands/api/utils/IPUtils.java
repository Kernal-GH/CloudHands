package com.antell.cloudhands.api.utils;

import java.net.InetAddress;
import java.net.UnknownHostException;

/**
 * Created by dell on 2017/4/17.
 */
public class IPUtils {

    private static final String regex =
            "^(1\\d{2}|2[0-4]\\d|25[0-5]|[1-9]\\d|[1-9])\\."
                    + "(1\\d{2}|2[0-4]\\d|25[0-5]|[1-9]\\d|\\d)\\."
                    + "(1\\d{2}|2[0-4]\\d|25[0-5]|[1-9]\\d|\\d)\\."
                    + "(1\\d{2}|2[0-4]\\d|25[0-5]|[1-9]\\d|\\d)$";

    public static final boolean valid(String ipStr) {

        if (!TextUtils.isEmpty(ipStr)) {

            if (ipStr.matches(regex)) {
                return true;
            }
        }

        return false;
    }

    public static final long ipv4LongLE(String ipStr) {
        String[] parts = ipStr.split("\\.");
        long ipNumbers = 0;

        for (int i = 0; i < 4; i++) {
            ipNumbers += Long.parseLong(parts[i]) << (24 - (8 * i));
        }

        return ipNumbers;
    }

    public static final long ipv4LongBE(String ipStr) {
        String[] parts = ipStr.split("\\.");
        long ipNumbers = 0;

        for (int i = 0; i < 4; i++) {
            ipNumbers += Long.parseLong(parts[i]) << ((8 * i));
        }

        return ipNumbers;
    }

    public static final String ipv4Str(long ipInt){

        StringBuffer sb = new StringBuffer();
        sb.append(String.valueOf(ipInt & 0xFF));
        sb.append(".");
        sb.append(String.valueOf((ipInt>>> 8) & 0xFF));
        sb.append(".");
        sb.append(String.valueOf(( ipInt>>> 16) & 0xFF));
        sb.append(".");
        sb.append(String.valueOf(( ipInt>>> 24) & 0xFF));
        return sb.toString();
    }

    public static final long reverse(long x){
        return  ((x & 0x000000ffL) << 24) |
                 ((x & 0x0000ff00L) << 8) |
                 ((x & 0x00ff0000L) >> 8) |
                 ((x & 0xff000000L) >> 24);

    }


    public static final boolean isInnerIPLE(long ip) {

        return (ip>>24 == 0xa) || (ip>>16 == 0xc0a8) || (ip>>22 == 0x2b0)||(ip>>24==0x7f&&((ip&0x1)==1))||(ip == 0);
    }

    public static final boolean isInnerIPBE(long ip) {

        return isInnerIPLE(reverse(ip));
    }

    public static final boolean isInnerIP(String ip){
        return isInnerIPLE(ipv4LongLE(ip));
    }

    public static final String ipv6Str(byte[] address){

        InetAddress addr;
        try {
            addr = InetAddress.getByAddress(null, address);
        } catch (UnknownHostException e) {
            return "0:0:0:0:0:ffff:";
        }
        if (addr.getAddress().length == 4) {
            // Deal with Java's broken handling of mapped IPv4 addresses.
            StringBuilder sb = new StringBuilder("0:0:0:0:0:ffff:");
            int high = ((address[12] & 0xFF) << 8) + (address[13] & 0xFF);
            int low = ((address[14] & 0xFF) << 8) + (address[15] & 0xFF);
            sb.append(Integer.toHexString(high));
            sb.append(':');
            sb.append(Integer.toHexString(low));
            return sb.toString();
        }
        return addr.getHostAddress();
    }

    public static void main(String[] args){

        System.out.println(ipv4Str(100772032l));
    }
}
