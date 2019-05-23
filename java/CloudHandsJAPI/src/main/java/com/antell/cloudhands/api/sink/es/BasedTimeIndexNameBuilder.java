package com.antell.cloudhands.api.sink.es;

import java.text.SimpleDateFormat;
import java.util.Date;

/**
 * Created by shajf on 2016/5/20.
 */
public class BasedTimeIndexNameBuilder implements IndexNameBuilder {

    private final static SimpleDateFormat dft = new SimpleDateFormat("yyyy.MM.dd");

    @Override
    public String build(String indexPrefix, String docType) {
        return  new StringBuilder().append(indexPrefix)
                .append("_")
                .append(dft.format(new Date()))
                .toString();
    }

}
