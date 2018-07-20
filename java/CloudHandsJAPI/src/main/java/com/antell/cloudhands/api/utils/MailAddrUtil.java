package com.antell.cloudhands.api.utils;

import java.util.Arrays;
import java.util.List;
import java.util.regex.Matcher;
import java.util.regex.Pattern;
import java.util.stream.Collectors;

/**
 * Created by lenovo on 2016/11/2.
 */
public final class MailAddrUtil {
    private static final String pattern = "[\\w!#$%&'*+/=?^_`{|}~-]+(?:\\.[\\w!#$%&'*+/=?^_`{|}~-]+)*@(?:[\\w](?:[\\w-]*[\\w])?\\.)+[\\w](?:[\\w-]*[\\w])?";
    private static  final Pattern regex = Pattern.compile(pattern);

    private MailAddrUtil(){

    }
    public final static String getMailAddrFromString(String str) {

        if(TextUtils.isEmpty(str))
            return "";

        Matcher matcher = regex.matcher(str);

        if (matcher.find()) {
            return matcher.group();
        }
        /* If not match, return the input string directory. */
        return str;
    }

    public final static List<String> toSimpleFromArray(List<String> list){

        return list.stream().map(e->getMailAddrFromString(e)).collect(Collectors.toList());
    }


}
