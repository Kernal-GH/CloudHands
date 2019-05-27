package com.antell.cloudhands.api.statistic;

import java.io.DataInput;
import java.io.IOException;

/**
 * Created by dell on 2018/6/8.
 */
public interface DataResultBuilder {

    void build(DataInput din) throws IOException;
}
