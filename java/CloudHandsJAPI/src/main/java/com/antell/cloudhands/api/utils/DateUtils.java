package com.antell.cloudhands.api.utils;

import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.Date;

/**
 * Created by dell on 2017/5/17.
 */
public class DateUtils {

    private final static SimpleDateFormat formatter = new SimpleDateFormat ("yyyy-MM-dd HH:mm:ss");

    public static String format(long time){

        Date dt = time<=0?new Date():new Date(time);

        return formatter.format(dt);
    }

    public static long toLong(String dateStr){

        if(TextUtils.isEmpty(dateStr))
            return 0;

        try {
           Date date =  formatter.parse(dateStr);
           return date.getTime();
        } catch (ParseException e) {
            e.printStackTrace();
        }

        return 0;
    }

}
