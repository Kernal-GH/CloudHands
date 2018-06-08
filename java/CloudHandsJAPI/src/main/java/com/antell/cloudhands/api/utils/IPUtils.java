package com.antell.cloudhands.api.utils;

/**
 * Created by dell on 2017/4/17.
 */
public class IPUtils {

    public static String ipv4Str(long ipInt){

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

    public static long reverse(long x){
        return  ((x & 0x000000ffL) << 24) |
                 ((x & 0x0000ff00L) << 8) |
                 ((x & 0x00ff0000L) >> 8) |
                 ((x & 0xff000000L) >> 24);

    }



}
