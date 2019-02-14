package com.antell.cloudhands.api;

import java.io.DataInput;
import java.io.IOException;

/**
 * Created by dell on 2018/6/15.
 */
public interface BinDataInput {

    void read(DataInput in) throws IOException;

}
