package com.antell.cloudhands.api.utils;

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

}
