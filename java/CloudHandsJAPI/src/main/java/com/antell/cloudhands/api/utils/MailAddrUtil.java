package com.antell.cloudhands.api.utils;

import java.util.regex.Matcher;
import java.util.regex.Pattern;

/**
 * Created by lenovo on 2016/11/2.
 */
public class MailAddrUtil {
    private static final String pattern = "[\\w!#$%&'*+/=?^_`{|}~-]+(?:\\.[\\w!#$%&'*+/=?^_`{|}~-]+)*@(?:[\\w](?:[\\w-]*[\\w])?\\.)+[\\w](?:[\\w-]*[\\w])?";
    private static  final Pattern regex = Pattern.compile(pattern);

    public static String getMailAddrFromString(String str) {
        Matcher matcher = regex.matcher(str);

        if (matcher.find()) {
            return matcher.group();
        }
        /* If not match, return the input string directory. */
        return str;
    }
}
